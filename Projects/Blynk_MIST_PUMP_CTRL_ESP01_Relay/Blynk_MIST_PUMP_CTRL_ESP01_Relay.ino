#include <stdio.h>

// CONNECTIONS:
// DS1302 CLK/SCLK  --> 4
// DS1302 DAT/IO    --> 16
// DS1302 RST/CE    --> 17
// DS1302 VCC       --> 5v
// DS1302 GND       --> GND
// LCD1602 SDA      --> A4
// LCD1602 SCLK     --> A5
// OUTPUT------------------------
#define Motor 14
#define Mist1 5
#define Mist2 6
#define Mist3 7
#define Mist4 8
// WaterCurtain 
#define WC1 9
#define WC2 10
#define WC4 11
// INPUT--------------------------
// EM_srply(비상급수 스위치)  --> 12      (내부 pull-up이용)
#define EM_srply 12
// WC_scheduled(계획수막실행) -->15   (내부 pull-up이용)
#define WC_scheduled 15
//interrupt루틴을 위한 [v]pin2,[v]pin3을 따로 예약하여야 한다
#define interrupt0 2  //interrupt 0번은 pin 2번에 해당함
#define interrupt1 3  //인터럽트 0번은 pin 3번에 해당함

//--------------------------------------------------------------------
#include <ThreeWire.h>
#include <RtcDS1302.h>
ThreeWire myWire(16,4,17); // IO(DAT), SCLK(CLK), CE(RST)
RtcDS1302<ThreeWire> Rtc(myWire);
#define countof(a) (sizeof(a) / sizeof(a[0]))
const char data[] = "what time is it";

#include <Wire.h> 
#include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
// SCLK = A5(19)
// SDA  = A4(18)

// Variables to be used with timers----------------------------------------------
unsigned long wateringSeconds = 90; // ==> Define Sample time in seconds to send data
unsigned long startWateringTime = 0;
unsigned long durationMilliSeconds = 0; //동당 관수 시간 (단위:초)
// 전역변수 정의 Mist공급을 하기 위한 
boolean MIST_STATE = false;                                               
unsigned int MIST[4] = { Mist1, Mist2, Mist3, Mist4};    
unsigned int GrnNm = 0; //미스트 밸브 array를 할당하기 위한 온실하우스 인수 변수  
// 계획 수막 밸브 작동 cf모터는 작동하지 않는다.-----------------------------------------
unsigned long WCScheduledTime = 300; //단위:초 각동마다 열리는 시간. 계획수막시간
unsigned long startWCTime = 0;
unsigned long WCDurationMilliSeconds = 0;
// 수막 공급을 위한 전역변수
boolean WC_STATE = false;
unsigned int WC[3] = {WC1,WC2,WC4};
unsigned int WCNm = 0;                                                                          
                 
boolean error;

void setup() {
  tab_setup();
}

void loop() {
  tab_loop();
}
