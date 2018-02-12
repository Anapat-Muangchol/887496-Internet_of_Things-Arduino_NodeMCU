/*
    This sketch demonstrates how to set up a simple HTTP-like server.
    The server will set a GPIO pin depending on the request
      http://server_ip/gpio/0 will set the GPIO2 low,
      http://server_ip/gpio/1 will set the GPIO2 high
    server_ip is the IP address of the ESP8266 module, will be
    printed to Serial when the module is connected.
*/

#include <ESP8266WiFi.h>

const char* ssid = "CYOS Pocket";
const char* password = "zpdc6709";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

//-------- DHT --------
#include <SimpleDHT.h>

#define pinDHT11 D3
SimpleDHT11 dht11;

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
  u8g2.begin();
  delay(10);

  // prepare GPIO2
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, 0);

  // Connect to WiFi network
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

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  int val;
  if (req.indexOf("/57160033/OFF") != -1)
    val = 0;
  else if (req.indexOf("/57160033/ON") != -1)
    val = 1;
  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }

  // Set GPIO2 according to the request
  digitalWrite(ledPin, val);

  client.flush();


  //read temperature
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(pinDHT11, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err); delay(1000);
    return;
  }


  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n<b>Your Student ID <i>57160033</i></b><br>";
  s += "You Send <b>";
  s += (val) ? "ON" : "OFF";
  s += "</b> LED Status<br>";
  s += "Now Temperature <b>";
  s += temperature;
  s += " *C</b><br>";
  s += "Now Humidity <b>";
  s += humidity;
  s += " %</b>";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

    //------- Display OLED ---------
  u8g2.firstPage();
  do {
    //print text
    u8g2.setFont(u8g2_font_astragal_nbp_tr);
    u8g2.setCursor(5, 15);
    u8g2.print("Student ID 57160033");

    u8g2.setCursor(5, 30);
    u8g2.print("LED Status ");
    if(req.indexOf("/ON") != -1){
      u8g2.print("ON");
    }else if(req.indexOf("/OFF") != -1){
      u8g2.print("OFF");
    }

    u8g2.setCursor(5, 45);
    u8g2.print("Temp ");
    u8g2.print(temperature);
    u8g2.print(" *C");

    u8g2.setCursor(5, 60);
    u8g2.print("Humidity ");
    u8g2.print(humidity);
    u8g2.print(" %");

  } while ( u8g2.nextPage() );
  
}

