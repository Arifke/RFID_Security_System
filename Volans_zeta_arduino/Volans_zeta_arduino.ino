/**
 * Test SysClock configuration on STM32
 * Disable USB Serial2
 * and use an external 
 * 
 * by Renzo Mischianti <www.mischianti.org>
 */
 #include "SafeStringReader.h"
 #include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <AsyncTimer.h>




 
// If you use generic STM32F103C8 
// you don't need this explicit declaration
// This is needed by bluepill specified version
HardwareSerial Serial2(USART4);   // PA3  (RX)  PA2  (TX)
HardwareSerial Serial3(USART2);


createSafeStringReader(sfReader, 512, "\r\n");

PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c); 





AsyncTimer t(25);




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


  sfReader.setTimeout(50); // set 1 sec timeout
  sfReader.flushInput(); // empty Serial RX buffer and then skip until either find delimiter or timeout
  sfReader.connect(Serial3); // read from Serial


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
}
 
void loop() {





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
 
            String card_body = "{\"card_number\":\"" +  card_id + "\"}";
            Serial2.print(card_body+"\r\n");
            //SerialBT.print(card_id+"\n");
          //ch9329Serial.print(card_id);
          /*
          for(int i=0; i<card_id.length(); i++) {
              sendKey(int(card_id[i]));
              releaseKeys();
          }
          */
          digitalWrite(PB12, HIGH);
          delay(50);
          digitalWrite(PB12, LOW);

          
          

          
         // last_card_id = card_id;
     }
     delay(50);
     if (sfReader.read()) {

    sfReader += "\r\n";
    Serial2.print(sfReader);
    digitalWrite(PB12, HIGH);
    delay(50);
    digitalWrite(PB12, LOW);
    
    sfReader.flushInput();
   }





  
}
void HandleTimer(void * pvParameters){
    for (;;) {
      t.handle();
    }
}
