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

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

/* Fill-in your Template ID (only if using Blynk.Cloud) */
//#define BLYNK_TEMPLATE_ID   "YourTemplateID"

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

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

#define GH1_V 8 //arduino pin 8
#define GH2_V 7 //arduino pin 7
#define GH3_V 6 //arduino pin 6
#define GH4_V 5 //arduino pin 5
#define MT_S 4  //arduino pin 4

int GH_V[5] = {0, GH1_V, GH2_V, GH3_V, GH4_V};
ESP8266 wifi(&EspSerial);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Virtual Button Programm
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int wateringTimeSec;    //1회 관수시간 sec
int periodMin;          //4동 전체 관주하며 돌아오는 관주 주기시간 min
bool GH[5] = {false}; //온실 각동의 밸브 ex) GH[1]은 1동의 valve 상태
bool MT = false;          //Motor 기동 스위치 on여부 수동일때만 고려
bool isAutoMode=false;  // isAutoMode에서는 MT상관 없이 자동모드 실행
bool prevAutoMode=false;     // prevAutoMode는 수동/자동 버튼의 이전 상태를 기억함.
unsigned long wateringStart;
unsigned long lastWateringTime;

BLYNK_WRITE(V0) //펌프기동
{
  //prevMT = MT;
  MT = param.asInt();
}

BLYNK_WRITE(V1) //온실1동
{
  GH[1] = param.asInt(); // Get value as integer
}

BLYNK_WRITE(V2) //온실2동
{
  GH[2] = param.asInt();
}

BLYNK_WRITE(V3) //온실3동
{
  GH[3] = param.asInt();
}

BLYNK_WRITE(V4) //온실4동
{
  GH[4] = param.asInt();
}

BLYNK_WRITE(V5) //1회당 관수 시간, 단위 sec
{
  wateringTimeSec = param.asInt();
}

BLYNK_WRITE(V6) //온실 전체의 관수 주기, 단위 min
{
  periodMin = param.asInt();
}

int whatGH[5] = {0}; //어떤 온실을 관수하는가?
int NoGH; //관수해야할 온실의 동수는 몇동인가?
BLYNK_WRITE(V7){  //수동 또는 누르면 자동
  isAutoMode = param.asInt();
  NoGH = 0;
  if(isAutoMode) {
    wateringStart = millis(); //자동 버튼이 눌러지면 wateringStart를 초기화
    digitalWrite(GH1_V, LOW);
    digitalWrite(GH2_V, LOW);
    digitalWrite(GH3_V, LOW);
    digitalWrite(GH4_V, LOW);
    digitalWrite(MT_S, LOW);
    int j = 1;  //whatGH[]의 인수로 쓰임. 
    for(int i = 1; i <= 4; i++){
      // whatGH[i]는 순차적으로 해야하는 관수동을 인수로 집어넣은 배열이다.
      // whatGH[i]의 인수가 0이면 i-1에서 관수가 끝이나는 의미이다.
      // whatGH[i]의 인수가 0이 아니라면, 그 인수가 지금 관수하는 동의 번호이다.
      // whatGH[i]의 인수와 i의 수자가 인수를 확인하며 관수해야 한다.
      if(GH[i] == true) { 
        whatGH[j] = i;
        j++;
        whatGH[j] = 0;
        NoGH++;
      } //조건부를 거치고 나면 whatGH[]의 인수는 관수해야할 동수를 나타낸다.
    }
  }
}
BLYNK_CONNECTED(){
  Blynk.syncAll();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// definition of fonction and subroutine
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MotorOn(){
  digitalWrite(MT_S, HIGH); // MT_S는 모터에 연결된 핀 번호
}
void MotorOff(){
  digitalWrite(MT_S, LOW);
}

//각 동마다 관수. thisGH가 1~4범위를 벗어나면 모든 동의 관수 밸브를 닫음
void wateringThisGH(int thisGH) 
{
  switch (thisGH)
  {
  case 1 : /* constant-expression */
    /* code */
    digitalWrite(GH1_V, HIGH);
    digitalWrite(GH2_V, LOW);
    digitalWrite(GH3_V, LOW);
    digitalWrite(GH4_V, LOW);
    break;
  case 2 :
    digitalWrite(GH1_V, LOW);
    digitalWrite(GH2_V, HIGH);
    digitalWrite(GH3_V, LOW);
    digitalWrite(GH4_V, LOW);
    break;
  case 3 :
    digitalWrite(GH1_V, LOW);
    digitalWrite(GH2_V, LOW);
    digitalWrite(GH3_V, HIGH);
    digitalWrite(GH4_V, LOW);
    break;
  case 4 :
    digitalWrite(GH1_V, LOW);
    digitalWrite(GH2_V, LOW);
    digitalWrite(GH3_V, LOW);
    digitalWrite(GH4_V, HIGH);
    break;
  default:
    digitalWrite(GH1_V, LOW);
    digitalWrite(GH2_V, LOW);
    digitalWrite(GH3_V, LOW);
    digitalWrite(GH4_V, LOW);
    break;
  }
}

void setup()
{
  pinMode(GH1_V, OUTPUT);
  pinMode(GH2_V, OUTPUT);
  pinMode(GH3_V, OUTPUT);
  pinMode(GH4_V, OUTPUT);
  pinMode(MT_S, OUTPUT);

  // Debug console
  Serial.begin(115200);

  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  Blynk.begin(auth, wifi, ssid, pass);

}


int wateringCount_th = 1; // 관수 주기에서 몇번째로 관수하고 있는지 확인
void loop()
{
  Blynk.run();
  if(!isAutoMode){  // 수동 모드
    digitalWrite(GH1_V, GH[1]);
    digitalWrite(GH2_V, GH[2]);
    digitalWrite(GH3_V, GH[3]);
    digitalWrite(GH4_V, GH[4]);
    digitalWrite(MT_S, MT);
  }  
  // 자동 모드 - millis()를 기준은 현재 관수 위치를 계속 계산해 내야되는 어려움이 있다.
  else {  // 자동 모드   
    //안전 판별 기준, (관수회수의 합*1회당 관수시간)과 관수주기를 비교. 작을때만 의미 있고 클때는 처음부터 다시 
    if(wateringTimeSec*((int)GH[1]+(int)GH[2]+(int)GH[3]+(int)GH[4]) > periodMin*60) {      
      return;
    }
    // (자동모드이며) 관수 주기, 개별관수 시간 그리고 관수 동수가 조건에 일치할 때만 다음의 조건식 안으로 들어간다.
    if((millis()-wateringStart) <= (unsigned long) (periodMin * 60 * 1000)){
      //whatGH[]와 (int)NoGH를 이용해서 루프 관수를 시작하는 process. 이 배열과 변수는 BLYNK_WRITE(V7)에 정의되어 있다.
      int argWhatGH;
      argWhatGH = (int)((millis()-wateringStart)/(wateringTimeSec*1000))+1;
      //계산한 값이 전체 관수해야할 온실 총수를 초과할 경우 관수는 종료하고 관수 주기만큼 기다린다.
      if(argWhatGH <= NoGH) { 
        wateringThisGH(whatGH[argWhatGH]);
        MotorOn();
      }
      else{ // 관수를 종료하고 관수주기만큼 기다린다.
        wateringThisGH(0);
        MotorOff();
      }   
      //관수 주기를 초과하면 다시 wateringStart를 초기화하고 loop()를 다시 시작한다.
      
    } 
    else { // 관수주기와 관수회수의 합을 비교. 클때 생기는 조건이다.
      wateringStart = millis();
    }
 }
}