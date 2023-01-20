#include <stdio.h>
#include "BylnkLink.h"
// CONNECTIONS:
// DS1302 CLK/SCLK  --> 4   취소
// DS1302 DAT/IO    --> 16  취소
// DS1302 RST/CE    --> 17  취소
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

/* RTC 제어 구문 무효화 -> ESP01로 대체
//--------------------------------------------------------------------
#include <ThreeWire.h>
#include <RtcDS1302.h>
ThreeWire myWire(16,4,17); // IO(DAT), SCLK(CLK), CE(RST)
RtcDS1302<ThreeWire> Rtc(myWire);
#define countof(a) (sizeof(a) / sizeof(a[0]))
const char data[] = "what time is it";
*/

#include  <Wire.h>
#include  <LiquidCrystal_I2C.h>
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

int EMS_count=0;
unsigned long EMS_last = millis();
void EMS()
{
  if(millis()-EMS_last >= 500){
    EMS_count = 0;
    EMS_last = millis();
  }
  if(EMS_count < 3){
    EMS_count += 1;
    Serial.print("Count is under"); Serial.println(EMS_count);
    return;
  }
  //MIST_STATE를 초기화
  MIST_STATE = false;
  //GrnNm를 초기화
  GrnNm = 0;
  digitalWrite(Motor, LOW);
  for(int i=0; i<4; i++) {
    digitalWrite(MIST[i], LOW);
  }
  WC_STATE = false;
  WCNm = 0;
  for(int i=0; i<3; i++){
    digitalWrite(WC[i], LOW);
  }
  EMS_count = 0;
  delay(100);
  /* 이하 코드를 실행시키면,  setup()이 실행되지 않아서 주석처리함
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Interrupt Pressed");*/
  Serial.println("Interrupt Button Pressed");
}

int NEXT_count=0;
unsigned long NEXT_last = millis();

void MIST_NEXT(){
  if(millis()-NEXT_last >= 500){
    NEXT_count = 0;
    NEXT_last = millis();
  }
  if(NEXT_count < 3){
    NEXT_count += 1;
    Serial.print("Count(MIST) is under"); Serial.println(NEXT_count);
    return;
  }
  //MIST_STATE가 멈춰있으면 바로 끝
  if(MIST_STATE == false) return;
  //한동 관수가 끝났다고 알림
  startWateringTime -= (wateringSeconds *1000);
  NEXT_count = 0;
  delay(100);
  /* 이하 코드를 실행시키면,  setup()이 실행되지 않아서 주석처리함
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Interrupt Pressed");*/
  Serial.println("Interrupt(MIST-NEXT) Button Pressed");
}

void tab_setup()
{
  //Pin에 Digital output 할당
  pinMode(Motor, OUTPUT);
  pinMode(Mist1, OUTPUT);
  pinMode(Mist2, OUTPUT);
  pinMode(Mist3, OUTPUT);
  pinMode(Mist4, OUTPUT);
  pinMode(WC1, OUTPUT);
  pinMode(WC2, OUTPUT);
  pinMode(WC4, OUTPUT);
  digitalWrite(Motor, LOW);
  digitalWrite(Mist1, LOW);
  digitalWrite(Mist2, LOW);
  digitalWrite(Mist3, LOW);
  digitalWrite(Mist4, LOW);
  digitalWrite(WC1, LOW);
  digitalWrite(WC2, LOW);
  digitalWrite(WC4, LOW);
  //Pin에 Digital input할당
  pinMode(EM_srply, INPUT);
  pinMode(WC_scheduled, INPUT);
  //input pin에 internal pull-up사용하기
  digitalWrite(EM_srply, HIGH);
  digitalWrite(WC_scheduled, HIGH);
  //interrupt pin 정의
  pinMode(interrupt0, INPUT);
  pinMode(interrupt1, INPUT);
 
  //For Liquid Crystal Display I2C
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  //-----------------------------------------------------------
  //인터럽트 처리루틴 : 인터럽트 처리는 FALLING일 때 호출
  //EMS(emergence stop)
  attachInterrupt(0, EMS, LOW);
  attachInterrupt(1, MIST_NEXT, LOW);
  
  //For initialize Real Time Clock DS1302
}

void tab_loop()
{
  start: //label 
  error=0;

  //interrupt routin때문에 한줄 추가
  if((digitalRead(EM_srply)==HIGH) && (MIST_STATE == false)){
    lcd.setCursor(0,0); lcd.print("Motor OFF                  ");
  }
  //수막 버튼이 눌러지고 MCU에서 수막상태가 없다면 지금 수막이 행해지고 있는 하우스동을 표시
  if((digitalRead(WC_scheduled)==HIGH) && (WC_STATE==false)){
    lcd.setCursor(0,1); lcd.print("Wtr_Crtn_Valve:"); lcd.print(WCNm);
  }
  //EM_srply핀의 입출력을 확인하고 바운싱 제거를 한 후, Motor_state를 결정한다.
  if((digitalRead(EM_srply) == LOW) && (MIST_STATE == false)) { //미스트 1회 비상관수
    //EM_srply는 pull_up되어 있는 상태임으로 입력은 LOW로 확인된다.
    MIST_STATE = true;
    //-----------------------------------------------------------
    //loop에서 각 밸브의 유지 간격 시간을 결정하기 위해 프로그램 시간을 저장
    startWateringTime = millis(); // starting the "program clock"
    Serial.println("EM_srply pressed");    
    delay(50);
    // 수막 밸브와 겹치지 않게 하기 위해
    WC_STATE = false;
    digitalWrite(WC[WCNm],LOW);
  }
  //만약 MIST_STATE가 참이라면 이제 미스트 관수를 시작해야 한다.
  //미스트 동작시간 안이면 모터와 동마다 밸브를 연다.
  if(MIST_STATE == true){
    durationMilliSeconds = millis()-startWateringTime; 
    //loop실행도중 보험삼아 실행시각에서 걸린 시간을 Monitoring한다.
    //unsigned long writeTimingSeconds = 17 
    // ==> Define Sample time in seconds to send data
    if(durationMilliSeconds < (wateringSeconds * 1000)) {
      digitalWrite(Motor, HIGH);
      lcd.setCursor(0,0);
      lcd.print("Motor ON");
      digitalWrite(MIST[GrnNm], HIGH);
      lcd.print(" House:");lcd.print(GrnNm+1);
    } 
    //미스트 관주가 정해진 시간에 이를때 미스트를 끄거나 다음 동으로 넘겨야 한다.
    else
      //마지막 동이 아닐 때, Motor는 그대로 두고 지금 온실 하우스 밸브는 닫고, 
      //하우스 넘버 올린다음,다음 온실하우스 밸브는 열고, startWateringTime 초기화
      if( GrnNm < 3) {
        digitalWrite(MIST[GrnNm], LOW);
        GrnNm += 1;
        digitalWrite(MIST[GrnNm], HIGH);
        lcd.setCursor(15,0); lcd.print(GrnNm+1);
        startWateringTime = millis();
      }      
      //다음 동으로 넘길 때 GrnNm가 마지막 동에 있으면 미스트를 끄고 GrnNm 동번호를 0으로 초기화한다
      else {
        digitalWrite(Motor, LOW);
        digitalWrite(MIST[GrnNm], LOW);
        lcd.setCursor(0,0); lcd.print("                 ");
        lcd.setCursor(0,0);
        lcd.print("Motor OFF");
        GrnNm = 0;  
        MIST_STATE = false;
      }
  }
  //WC_scheduled핀의 입출력을 확인하고 바운싱 제거
  //미스트 관수 중이면 WC_scheduled 핀 입력을 무시
  if((digitalRead(WC_scheduled) == LOW) && (WC_STATE == false) && (MIST_STATE == false)){
    //WC_STATE는 digitalWrite(WC_STATE, HIGH)되어서 pull up되어 있는 상태임
    WC_STATE = true;
    startWCTime = millis();   //계획 수막 시작을 위하여 system time을 startWCTime에 할당
    Serial.println("WC_scheduled pressed");
    delay(10);
  }
  //만약 WC_STATE가 참이라면 이제 동마다 수막밸브를 열고 닫는다.
  if(WC_STATE == true){
    WCDurationMilliSeconds = millis()-startWCTime;
    if(WCDurationMilliSeconds < (WCScheduledTime * 1000)){
      digitalWrite(WC[WCNm], HIGH);
      lcd.setCursor(0,1);
      lcd.print("Wtr_Crtn_Valve:"); 
      if(WCNm==2) {
        lcd.print(WCNm+2);
      } else {
        lcd.print(WCNm+1);
      }
    }
    else if(WCNm < 2){
      digitalWrite(WC[WCNm], LOW);
      WCNm += 1;
      digitalWrite(WC[WCNm], HIGH);
      startWCTime = millis();
    }
    else {
      digitalWrite(WC[WCNm], LOW);
      WCNm = 0;
      WC_STATE = false;
      lcd.setCursor(15,1); lcd.print("0");
    }
  }
  Serial.println();
  delay(100); // ten seconds
}
/*Blynk Sever에서 virtual pin V0는 Motor on이다.
  Mist Motor가 작동하면, 각 온실의 밸브는 모두 on되어야 하고
  Mist Motor가 정지하면, 각 온실의 밸브는 모두 off되어야 한다.
  interrupts도 함께 제어한다.
*/
int blynkState = 0;
BLYNK_WRITE(V0)
{
    int motorState = param.asInt();
    blynkState = motorState;
    if(motorState == 1){  //원격에서 스위치가 눌러졌다.
      noInterrupts();
      digitalWrite(Mist1, HIGH);
      digitalWrite(Mist2, HIGH);
      digitalWrite(Mist3, HIGH);
      digitalWrite(Mist4, HIGH);
      digitalWrite(Motor, HIGH);
    } else {  //원격에서 스위치가 꺼졌다.
      interrupts();      
      digitalWrite(Motor, LOW);
      digitalWrite(Mist1, LOW);
      digitalWrite(Mist2, LOW);
      digitalWrite(Mist3, LOW);
      digitalWrite(Mist4, LOW);
    }
}
void Blynk_Setup(void)
{
  EspSerial.begin(ESP8266_BAUD);
  delay (10);
  Blynk.begin(auth, wifi, ssid, pass,"blynk-cloud.com");
}

void Blynk_Loop()
{
  Blynk.run();
}

void setup() {
  // Debug console
  Serial.begin(115200);

  delay(10);

  Blynk_Setup();
  tab_setup();
}

void loop() {
  Blynk_Loop();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
  tab_loop(); //Blynk에서 신호가 없을 때 loop를 돌린다. 신호 충돌에 대해서는 아직 연구하지 않았다.
}