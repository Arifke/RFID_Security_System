#include "SafeStringReader.h"
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <AsyncTimer.h>

unsigned long previousMillis = 0; 
const long interval = 3000;
String nfc_last_card_id;

HardwareSerial Serial2(USART4);   // PA3  (RX)  PA2  (TX)
HardwareSerial Serial3(USART2);

createSafeStringReader(sfReader, 512, "\r\n");
TwoWire Wire1(PB7, PB6);
PN532_I2C pn532i2c(Wire1);
PN532 nfc(pn532i2c); 

AsyncTimer t(5);
//TaskHandle_t Task1;
String last_card_id;

extern "C" void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL3;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

void setup() {
  pinMode(PB12,OUTPUT);
  Serial2.begin(115200);
  Serial3.begin(9600);

  Wire1.begin();


  sfReader.setTimeout(10); // set 1 sec timeout
  sfReader.flushInput(); // empty Serial RX buffer and then skip until either find delimiter or timeout
  sfReader.connect(Serial3); // read from Serial
  t.setup();
  
  nfc.begin();
  uint32_t versiondata_enterance = nfc.getFirmwareVersion();
  if (! versiondata_enterance) {
  } else { 
    //nfc_ready = true;
    nfc.SAMConfig();
  }
  
  while (!Serial2){ delay(100); }
  Serial2.println("START!");
  digitalWrite(PB12,HIGH);
  delay(100);
  digitalWrite(PB12,LOW);
  delay(100);
  digitalWrite(PB12,HIGH);
  delay(100);
  digitalWrite(PB12,LOW);
  delay(100);


  t.setInterval(clear_last_card_id, 3000);
}
 
void loop() {
  
  
  if(sfReader.read()) {
    sfReader += "\r\n";
    Serial2.print(sfReader);
    digitalWrite(PB12, HIGH);
    delay(50);
    digitalWrite(PB12, LOW);
    sfReader.flushInput();
   }

   uint8_t nfc_success;
   uint8_t nfc_uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
   uint8_t nfc_uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
   nfc_success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, nfc_uid, &nfc_uidLength);
   
   if (nfc_success) {
    unsigned long UID_unsigned;
    UID_unsigned =  nfc_uid[3] << 24;
    UID_unsigned += nfc_uid[2] << 16;
    UID_unsigned += nfc_uid[1] <<  8;
    UID_unsigned += nfc_uid[0];
    String card_id = (String) UID_unsigned;
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval){
      previousMillis = currentMillis;
      clear_last_card_id();
      }
      if(card_id == last_card_id){
        
        }
        else{
          String card_body = "{\"card_number\":\"" +  card_id + "\"}";
          Serial2.print(card_body+"\r\n");
 
          
          digitalWrite(PB12, HIGH);
          delay(50);
          digitalWrite(PB12, LOW);

          last_card_id = card_id;
          }   
    }       
}

void clear_last_card_id() {
  last_card_id = String();
}
