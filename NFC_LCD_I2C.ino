#include<SPI.h>                   // RC522 haberlesmesi icin SPI kutuphanesini ekliyoruz
#include<Wire.h>                  // LCD ekran I2C modülü ile kullanilmasi icin gerekli olan kutuphane
#include<Servo.h>                 // Servo kütüphanemizi tanımlıyoruz.
#include<MFRC522.h>               // RC522 kutuphanesi ekliyoruz
#include<LiquidCrystal_I2C.h>     // LCD ekran icin gerekli olan kutuphane
LiquidCrystal_I2C lcd(0x27,16,2);

#define SS_PIN 10                  //RC522 SS(SDA) pinini tanimliyoruz
#define RST_PIN 9                  // RC522 RESET pinini tanimliyoruz
#define Servo_PIN 7                //Servo motor pinini tanımlıyoruz.
#define beep_pin 8                 // buton pini
#define led_red 6
#define led_green 5
MFRC522 rfid(SS_PIN, RST_PIN);
Servo motor;


void setup() 
{
Serial.begin(9600);
pinMode(beep_pin,OUTPUT);
digitalWrite(beep_pin,LOW);
motor.attach(Servo_PIN);
lcd.init();
lcd.backlight();
lcd.setCursor(2,0);
lcd.print("Hosgeldiniz");
delay(1500);
lcd.setCursor(2,1);
lcd.print("Sistem Hazir");
delay(1500);
lcd.clear();

SPI.begin();                     //SPI haberlesmesini baslatıyoruz
rfid.PCD_Init();                 //RC522 sensorunu calıstırıyoruz
Serial.println("Kart Okutunuz");
Serial.println();

}

void loop() 
{
digitalWrite(beep_pin,LOW);
lcd.clear();
lcd.setCursor(1,0);
lcd.print("Kart Okutunuz");
delay(300);

if(! rfid.PICC_IsNewCardPresent())      // Kartı okuttugumuzda surekli olarak ID'nin gelmemesi icin bu komutu kullanıyoruz
{
return;
}

if(!rfid.PICC_ReadCardSerial())         // Kart okunmadıgı zaman bekiyoruz
{
  return;
}

Serial.print("UID No:");
String content="";
byte letter;                             //byte 0-255 arasidanki degerleri tutabilen 8bitten olusan degiskenlerdir

for(byte i=0;i<rfid.uid.size;i++)
{
  Serial.print(rfid.uid.uidByte[i]<0x10 ? " 0": " ");
  Serial.print(rfid.uid.uidByte[i],HEX);
  content.concat(String(rfid.uid.uidByte[i]<0x10 ? " 0" : " "));  //parantez icindeki parametreleri string'e ekler
  content.concat(String(rfid.uid.uidByte[i],HEX));
}

Serial.println();
Serial.print("Mesaj : ");
content.toUpperCase();               //string parametrelerini buyuk harfle yazar

if(content.substring(1) == "43 E0 53 32" || content.substring(1) == "32 95 D5 08")
//content.substring(1) nesnesi, okunan string parametresini parantez icindeki degerden baslayarak yeniden yazar.
//content.substring(4) olsaydi stringin 4.degerinde sonra okuyup bunu kendi icine kaydedecektir
{
      digitalWrite(beep_pin,HIGH);
      digitalWrite(led_green,HIGH);
      delay(300);
      digitalWrite(beep_pin,LOW);
      digitalWrite(led_green,LOW);
      delay(100);
      lcd.setCursor(0 ,0);
      lcd.print("ID : ");
      lcd.print(content.substring(1));
      lcd.setCursor(1, 1);
      lcd.print("Giris Basarili");
      Serial.println("Giris Basarili");
      delay(2000);
      lcd.clear();
      motor.write(180);                 //Servo motoru 180 dereceye getiriyoruz.
      delay(3000);
      motor.write(0);                   //Servo motoru 0 dereceye getiriyoruz.
      delay(1000);
}

 else
    {
      digitalWrite(beep_pin,HIGH);
      digitalWrite(led_red,HIGH);
      delay(500);
      digitalWrite(led_red,LOW);
      digitalWrite(beep_pin,LOW);
      delay(500);
      lcd.setCursor(0 ,0);
      lcd.print("ID : ");
      lcd.print(content.substring(1));
      lcd.setCursor(0, 1);
      lcd.print("Giris Basarisiz");
      Serial.println("Giris Basarisiz");
      delay(1500);
      lcd.clear();
    }

}
