/*
 * Ultrasonic Simple
 * ---------------------    ---------------------
 * | HC-SC04 | Arduino |    | 3 pins  | Arduino |
 * ---------------------    ---------------------
 * |   Vcc   |   5V    |    |   Vcc   |   5V    |
 * |   Trig  |   12    | OR |   SIG   |   13    |
 * |   Echo  |   13    |    |   Gnd   |   GND   |
 * |   Gnd   |   GND   |    ---------------------
 * ---------------------
 */

//-------- Ultrasonic --------
#include <Ultrasonic.h>
Ultrasonic ultrasonic(12, 13);

//-------- OLED --------
//#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display

//-------- DHT --------
#include <SimpleDHT.h>

int pinDHT11 = 11;
SimpleDHT11 dht11;
//----------------------

// Set range here
float range = 100.0;

#define ledPin 2

boolean isFoundOld;
byte temperatureOld;

void setup() {
  Serial.begin(9600);
  u8g2.begin();
  pinMode(ledPin, OUTPUT);
  
  // read temperature.
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(pinDHT11, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
    return;
  }
  temperatureOld = temperature;

  // read distance.
  float distance = ultrasonic.distanceRead();
  if(distance < range){
    Serial.println("Found Human");
    Serial.print((int)temperature); Serial.println(" * C       ");
    digitalWrite(ledPin, HIGH);
    
    u8g2.firstPage();
    do {
      u8g2.setFont(u8g2_font_ncenB10_tr);
      u8g2.setCursor(5, 24);
      u8g2.print("Found Human");
      
      u8g2.setCursor(5, 50);
      u8g2.print((int)temperature);
      u8g2.setCursor(30, 50);
      u8g2.print("* C");
      
    } while ( u8g2.nextPage() );
    
    isFoundOld = true;
  }else{
    Serial.println("!!! Not Found !!!");
    Serial.print((int)temperature); Serial.println(" * C       ");
    digitalWrite(ledPin, LOW);
    
    u8g2.firstPage();
    do {
      u8g2.setCursor(5, 24);
      u8g2.print("!!! Not Found !!!");
      
      u8g2.setCursor(5, 50);
      u8g2.print((int)temperature);
      u8g2.setCursor(30, 50);
      u8g2.print("* C");
      
    } while ( u8g2.nextPage() );

    isFoundOld = false;
  }

}

void loop() {

  // read temperature.
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(pinDHT11, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
    return;
  }

  // read distance.
  float distance = ultrasonic.distanceRead();
  
  boolean isFound;
  if(distance < range){
    isFound = true;
  }else{
    isFound = false;
  }
  
  if(isFoundOld != isFound || temperatureOld != temperature){
    
    if(isFound){
      Serial.println("Found Human");
      Serial.print((int)temperature); Serial.println(" * C       ");
      digitalWrite(ledPin, HIGH);
    
      u8g2.firstPage();
      do {
        u8g2.setFont(u8g2_font_ncenB10_tr);
        u8g2.setCursor(5, 24);
        u8g2.print("Found Human");
      
        u8g2.setCursor(5, 50);
        u8g2.print((int)temperature);
        u8g2.setCursor(30, 50);
        u8g2.print("* C");
        
      } while ( u8g2.nextPage() );
      
    }else{
      Serial.println("!!! Not Found !!!");
      Serial.print((int)temperature); Serial.println(" * C       ");
      digitalWrite(ledPin, LOW);
    
      u8g2.firstPage();
      do {
        u8g2.setFont(u8g2_font_ncenB10_tr);
        u8g2.setCursor(5, 24);
        u8g2.print("!!! Not Found !!!");
      
        u8g2.setCursor(5, 50);
        u8g2.print((int)temperature);
        u8g2.setCursor(30, 50);
        u8g2.print("* C");
        
      } while ( u8g2.nextPage() );
    
    }
    isFoundOld = isFound;
  }

  isFoundOld = isFound;
  temperatureOld = temperature;
  
  delay(100);
}
