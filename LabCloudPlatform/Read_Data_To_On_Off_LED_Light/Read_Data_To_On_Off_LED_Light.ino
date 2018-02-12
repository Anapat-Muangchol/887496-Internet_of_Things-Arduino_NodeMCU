#include <ESP8266WiFi.h>

const char* ssid = "CYOS Pocket"; //ใส่ SSID WiFi ที่ต้องการเชื่อมต่อ
const char* password = "envm5597"; //ใส่ password ของ WiFi ที่ต้องการเชื่อมต่อ
const char* host = "api.thingspeak.com"; // ชื่อ host ที่ต้องการติดต่อ
const char* url = "/channels/349430/fields/4/last"; // path ที่ต้องการเรียก

int lengthData = 0; //ตัวแปรนี้จะเก็บขนาดของข้อมูลที่ทาง Server มีการ response กลับมา
int data = 0; // ตัวแปรนี้จะเก็บค่าข้อมูลที่เราต้องการใช้งาน

String line = ""; // ตัวแปรนี้จะเก็บค่าทั้งหมดที่อ่านได้จาก Server

int led1 = D2; //กำหนดขา GPIO OUTPUT ซึ่ง pin นี้จะตรงกับ D2

const int PORT = 80; // ใช้ port 80

WiFiServer server(80); // สร้าง object จาก คลาส WiFiServer กำหนด port 80

void setup() {
  Serial.begin(9600); //ใช้ buadrate 9600
  delay(10);

  pinMode(led1, OUTPUT); // กำหนด ขา ให้เป็น ขา OUTPUT

  Serial.println("\n\n");
  Serial.print("Connecting to ");
  Serial.println(ssid);  // แสดงข้อความออกทาง Serial ว่า Connecting to (SSID ที่เชื่อมต่อ)
  WiFi.begin(ssid, password);  // เชื่อมต่อ wifi จาก SSID และ password ที่เรากำหนด

  while (WiFi.status() != WL_CONNECTED) { // ถ้าหากยังเชื่อมต่อกับ wifi ยังไม่ได้ก็จะแสดงข้อความ ... ไปเรื่อย ๆ
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected!!");
  Serial.print("IP address : ");
  Serial.println(WiFi.localIP());  // แสดง IP address ที่ได้รับจาก wifi ที่เราเชื่อมต่อ
}

void loop() {
  delay(5000); //delay ไว้ 5 วิ
  Serial.print("Connecting to ");
  Serial.println(host); //แสดงข้อความทาง Serial ว่า Connecting to (host ที่ต้องการเชื่อมต่อ)

  WiFiClient client = server.available(); // สร้าง client ขึ้นมาจาก WiFiClient และกำหนดให้เท่ากับ Server.available() เพื่อไว้เช็คเวลามีข้อมูลส่งกลับมา

  Serial.print("Requesting URL : ");
  Serial.println(url); // แสดง url ที่เรา request ไป

  if (!client.connect(host, PORT)) {
    Serial.print("."); //ในเงื่อนไขนี้เช็คว่าถ้าไม่ได้เชื่อมต่อกับ host อยู่ให้แสดง . ออกมา
    return;
  } else {
    // และถ้าหากเชื่อมต่ออยู่ให้ส่ง http request ไปที่ server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    delay(50);
    Serial.println("SEND !!");  // เมื่อส่งไปแล้วให้แสดงข้อความ SEND !! ออกมา
  }

  while (!client.available()) { //ถ้าหากไม่มี response ตอบกลับมา ก็จะให้วนลูป จนกว่าจะมี responce ตอบกลับมา
  }

  while (client.available()) { // และเมื่อมี response ตอบกลับมาก็จะให้เข้า loop while แล้วทำการเก็บค่าทั้งหมดไว้ที่ line
    line += (char)client.read();
  }

  Serial.println();
  Serial.println("GET SUCCESSFULLY !!"); // เมื่อทำการเก็บค่าไว้หมดแล้วก็จะแสดง GET SUCCESSFULLY ออกมา

  Serial.println("=========== line ===========");
  Serial.println(line);
  Serial.println("============================");
  
//  convertDataToInt(&lengthData , &data , line); // ใน function นี้เขียนขึ้นมาเพื่อตัดข้อความบางส่วนออก เอาเฉพาะที่เราจำเป็นต้องใช้งานจริง ๆ ก็จะมี lengthData และ data

//  String s = (String)"ID " + lengthData + " data " + data; // สร้างตัวแปร String มาดูผลลัพธ์ว่าข้อมูลที่ได้จาก Server
//  Serial.println(s); // แสดงข้อมูลที่ผ่านการ filter เรียบร้อยแล้ว

  data = line.substring(line.length() - 2).toInt();
  switch (data) { // แล้วก็ต่อมาจะเป็นเงื่อนไขกันเราก็เช็คกับตัวแปร data ว่าเป็นค่าอะไรบ้าง เพื่อที่เราจะเอามาทำเป็นเงื่อนไขในการเปิดปิด LED
    case 1:
      digitalWrite(led1, HIGH); // ถ้าเป็น 1 led1 ติด
      break;
    case 0:
      digitalWrite(led1, LOW); // ถ้าเป็น 2 led1 ดับ
      break;
  }

  line = ""; // เมื่อจบ process ทุกอย่างแล้ว ก็เคลียข้อมูลในตัวแปร line ให้เท่ากับ "" เพื่อรอรับค่าใหม่ใน loop หน้า
  Serial.println();
  Serial.println("Closing Connection"); //แสดงข้อความ Closing Connection
}

//void convertDataToInt(int* id_ , int* data_ , String rawData) { // พารามิเตอร์ ก็จะมี pointer id_ , pointer data_ , ค่าตัวแปรดิบที่อ่านได้จากที่ server response กลับมา
//  //Serial.println(rawData); // ลบ comment ออก ถ้าต้องการดูว่าค่าที่ response กลับมามีอะไรบ้าง
//  String dataSub = rawData.substring(rawData.indexOf("close") + 9); //สร้างตัวแปรมาเพื่อ substring เฉพาะข้อมูลที่เราต้องการใช้จริง ๆ
//  Serial.println(dataSub.length()); // ลบ comment ออก ถ้าต้องการดูขนาดข้อมูลหลังจากที่เรา substring จากข้อมูลทั้งหมดแล้ว
//  Serial.println(dataSub); //ลบ comment ออก ถ้าต้องการดูว่าข้อมูลที่เรา substring ออกมามีข้อมูลเป็นอย่างไร
//
//  *id_ = dataSub.substring(0, 2).toInt(); // ทำการแปลงข้อมูลจาก String เป็น int ไปเก็บข้อมูลไว้ที่ตัวแปร pointer เพื่อเปลี่ยนค่าให้กับตัวแปรที่เราอ้างอิง ก็คือตัวแปร lengthData นั่นเอง
//  *data_ = dataSub.substring(3 , 3 + *id_).toInt(); // ทำการแปลงข้อมูลจาก String เป็น int ไปเก็บข้อมูลไว้ที่ตัวแปร pointer เพื่อเปลี่ยนค่าให้กับตัวแปรที่เราอ้างอิง ก็คือตัวแปร data
//  data = dataSub.substring(0, 1).toInt();
//}

