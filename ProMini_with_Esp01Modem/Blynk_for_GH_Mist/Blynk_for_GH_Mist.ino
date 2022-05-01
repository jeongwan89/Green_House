/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************

  This example shows how to use ESP8266 Shield (with AT commands)
  to connect your project to Blynk.

  WARNING!
    It's very tricky to get it working. Please read this article:
    http://help.blynk.cc/hardware-and-libraries/arduino/esp8266-with-at-firmware

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/


/* Fill-in your Template ID (only if using Blynk.Cloud) */
//#define BLYNK_TEMPLATE_ID   "YourTemplateID"


#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "zM3g35YO8NtLM5S03VYnMLkRd3BNynQZ";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "FarmMain5G";
char pass[] = "wweerrtt";

// Hardware Serial on Mega, Leonardo, Micro...
//#define EspSerial Serial1

// or Software Serial on Uno, Nano...
#include <SoftwareSerial.h>
SoftwareSerial EspSerial(2, 3); // RX, TX

// Your ESP8266 baud rate:
#define ESP8266_BAUD 9600

#define GH1_V 8
#define GH2_V 7
#define GH3_V 6
#define GH4_V 5
#define MT_S 4

ESP8266 wifi(&EspSerial);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Virtual Button Programm
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int wateringTimeSec;  //1회 관수시간 sec
int periodMin;        //4동 전체 관주하며 돌아오는 관주 주기시간 min
bool GH1=false, GH2=false, GH3=false, GH4=false;  //Device의 버튼이 눌러져있는지 아닌지 확인하는 변수
bool MT=false;        //Motor 기동 스위치 on여부,
//bool prevMT=false;     // prevMT는 그전 모터 기동 상태를 확인하는 변수
unsigned long wateringStart;
unsigned long lastWateringTime;

BLYNK_WRITE(V5) //1회당 관수 시간, 단위 sec
{
  wateringTimeSec = param.asInt();
}

BLYNK_WRITE(V6) //온실 전체의 관수 주기, 단위 min
{
  periodMin = param.asInt();
}
BLYNK_WRITE(V1) //온실1동
{
  GH1 = param.asInt(); // Get value as integer
}

BLYNK_WRITE(V2) //온실2동
{
  GH2 = param.asInt();
}
BLYNK_WRITE(V3) //온실3동
{
  GH3 = param.asInt();
}
BLYNK_WRITE(V4) //온실4동
{
  GH4 = param.asInt();
}
BLYNK_WRITE(V0) //펌프기동
{
  //prevMT = MT;
  MT = param.asInt();
}
void MotorOn(){
  digitalWrite(MT_S, HIGH); // MT_S는 모터에 연결된 핀 번호
}
void MotorOff(){
  digitalWrite(MT_S, LOW);
}
void StartingSetup()  //모터 기동처리 setup
{
  wateringStart = millis();
}
void CyclingMist()    //작동 루프를 돌린다.
{
  if(wateringStart < millis() && millis() < (wateringStart + wateringTimeSec*1000)) {
    digitalWrite(GH1_V, GH1);  // GH1은 Blynk Server에서 오는 V1의 값
    digitalWrite(GH2_V, LOW);
    digitalWrite(GH3_V, LOW);
    digitalWrite(GH4_V, LOW);
  }
  if((wateringStart + wateringTimeSec*1000) < millis() && millis() < (wateringStart + wateringTimeSec*1000*2)) {
    digitalWrite(GH1_V, LOW);
    digitalWrite(GH2_V, GH2); // GH2_V는 #define 구문에서 정의된 GH2의 밸브에 연결된 핀 번호
    digitalWrite(GH3_V, LOW);
    digitalWrite(GH4_V, LOW);
  }
  if((wateringStart + wateringTimeSec*1000*2) < millis() && millis() < (wateringStart + wateringTimeSec*1000*3)) {
    digitalWrite(GH1_V, LOW);
    digitalWrite(GH2_V, LOW);
    digitalWrite(GH3_V, GH3);
    digitalWrite(GH4_V, LOW);
  }
  if((wateringStart + wateringTimeSec*1000*3) < millis() && millis() < (wateringStart + wateringTimeSec*1000*4)) {
    digitalWrite(GH1_V, LOW);
    digitalWrite(GH2_V, LOW);
    digitalWrite(GH3_V, LOW);
    digitalWrite(GH4_V, GH4);
  }
  if((millis()-wateringStart) > periodMin*60*1000)
    wateringStart = millis();
}
void EndingSetup()    //모터 종료처리 setup
{
  digitalWrite(GH1_V, LOW);
  digitalWrite(GH2_V, LOW);
  digitalWrite(GH3_V, LOW);
  digitalWrite(GH4_V, LOW);
}


void setup()
{
  // Setup Pin mode
  pinMode(GH1_V, OUTPUT); // GH1
  pinMode(GH2_V, OUTPUT); // GH2
  pinMode(GH3_V, OUTPUT); // GH3
  pinMode(GH4_V, OUTPUT); // GH4
  pinMode(MT_S, OUTPUT); // Motor

  // Debug console
  Serial.begin(115200);

  delay(10);

  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  Blynk.begin(auth, wifi, ssid, pass);
}

void loop()
{
  Blynk.run();
  if(MT == true){   //작동 루프를 돌려야한다.
    if(wateringTimeSec*4 <= periodMin*60) {
      CyclingMist();  // 이 함수는 valve를 작동시키는 판별조건이다.
      MotorOn();
    }
  }
  else {             //모터를 종료한다.
    EndingSetup();
    MotorOff();
  }
}
