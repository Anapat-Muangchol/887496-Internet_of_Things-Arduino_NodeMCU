/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <ESP8266WiFi.h>

const char* ssid = "CYOS Pocket";
const char* password = "zpdc6709";

const char* host = "192.168.43.9";
const char* streamId   = "....................";
const char* privateKey = "....................";


//-------- DHT --------
#include <SimpleDHT.h>

#define pinDHT11 D3
SimpleDHT11 dht11;

//-------- Ultrasonic --------
#include <Ultrasonic.h>
Ultrasonic ultrasonic(D5, D6);


//-------- OLED --------
//#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ D2, /* data=*/ D1, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display


//----------------------

#define ledPin D4


void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  u8g2.begin();
  
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

int value = 0;

void loop() {
  
  //-------- Ultrasonic --------
  float distance = ultrasonic.distanceRead();
  if(distance < 10){
    digitalWrite(ledPin, HIGH);
  }else{
    digitalWrite(ledPin, LOW);
  }
  
  ++value;

  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 8765;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }



  //read temperature
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(pinDHT11, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
    return;
  }

  //--------- Display OLED ---------
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.setCursor(5, 20);
    u8g2.print("Humidity:");
    u8g2.print((float)humidity);

    u8g2.setCursor(5, 40);
    u8g2.print("Temp:");
    u8g2.print((float)temperature);
    
    u8g2.setCursor(5, 60);
    u8g2.print("Distance:");
    u8g2.print((int)distance);
    
  } while ( u8g2.nextPage() );
  
  // We now create a URI for the request
  String url = "Humidity:";
  url += (float)humidity;
  url += ",Temperature:";
  url += (float)temperature;
  url += ",Distance:";
  url += (int)distance;
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.println(url);
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");

  delay(2000);
}

