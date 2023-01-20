/*
  The circuit:
   LCD RS pin to digital pin 12
   LCD Enable pin to digital pin 11
   LCD D4 pin to digital pin 5
   LCD D5 pin to digital pin 4
   LCD D6 pin to digital pin 3
   LCD D7 pin to digital pin 2
   LCD R/W pin to ground
   LCD VSS pin to ground
   LCD VCC pin to 5V
   10K resistor:
   ends to +5V and ground
   wiper to LCD VO pin (pin 3)
*/

// include the library code :
#include <LiquidCrystal.h>

//정완 Define
//      변수    핀번호
#define MOTOR   7
#define VAL1    8
#define VAL2    9
#define VAL3   10
#define SWS    A3
#define MAINPUMP 6

#define SW1_AUTOFLUSH
#define SW2_AUTOFEED
#define SW3_STOP

#define TIMEARGUMENT (60000)

//환경 정의
//관수시간
#define WATERING 2
//휴식시간
#define INTERVAL 5
//총관수시간
#define TOTALWATERING 15
int key_sw = 0;
int button = 0;
int prebutton = 0;
int amax = 0;
int amin = 1024;
/*함수 정의 구간*/
int assignSwitchNumber(int pinNo);
void initializeLCD(void);
void doNothing(void);
void doInitailState(void);
void doMotorOn(void);
void printState(unsigned long watering, unsigned long interval);
void watering(unsigned long feed, unsigned long interval, unsigned long duration);
void reserveReservoir(void);
//정완 Define 끝

//initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

/* 정완 초기화
    pin 6 : digital input, 내부풀업저항사용
    pin 7 : digital input, 내부풀업저항사용
*/

// the setup function runs once when you press reset or power the board
void setup() {
  //정완 초기화함수
  pinMode (VAL1, OUTPUT);
  pinMode (VAL2, OUTPUT);
  pinMode (VAL3, OUTPUT);
  pinMode (MOTOR, OUTPUT);
  pinMode (MAINPUMP, OUTPUT);

  Serial.begin(9600);

  //정완 초기화함수 끝

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  initializeLCD();
  //정완 밸브 및 모터 초기화
  doInitialState();
  //정완 밸브 및 모터 초기화 끝
}

// the loop function runs over and over again forever
void loop() {

  // Analog input 확인
  key_sw = analogRead(SWS); //Z-high일 경우를 대비하여 한번 더 호출
  button = assignSwitchNumber(SWS);
  /*
    //button에 대입하기 위한 key_sw값 판별식
    if(prebutton == button) {
      if(amax < key_sw) amax=key_sw;
      if(amin > key_sw) amin=key_sw;
    } else {
      amax=0;
      amin=1024;
    }
    prebutton = button;

    lcd.setCursor(0,0);
    lcd.print("max=    min=    ");
    lcd.setCursor (4,0);
    lcd.print(amax);
    lcd.setCursor (12,0);
    lcd.print(amin);
    lcd.setCursor (0,1);
    lcd.print("SW_value=");
    lcd.print(button);
    lcd.setCursor (11,1);
    lcd.print("     ");
    lcd.setCursor (11,1);
    lcd.print(key_sw);
  */

  switch (button) {
    case 4: //시간 관주
      initializeLCD();
      watering(WATERING, INTERVAL, TOTALWATERING);
      break;
    case 2: //탱크에 물받기
      initializeLCD();
      reserveReservoir();
      break;
    case 1: //정지
      initializeLCD();
      doNothing();
      break;
    default:  //일반적인 상태
      doInitialState();
  }
}


//정완 함수 정의
int assignSwitchNumber(int pinNo)
// pinNo : 스위치에 연결된 아날로그 핀 번호
// returne : 판단 조건에 따른 스위치 배정
{
  int key_sw;
  analogRead(pinNo);
  key_sw = analogRead(pinNo);
  if (key_sw >= 890) {
    return 7 ;
  }
  else if (key_sw >= 825) {
    return 6 ;
  }
  else if (key_sw >= 780) {
    return 5 ;
  }
  else if (key_sw >= 750) {
    return 3 ;
  }
  else if (key_sw >= 650) {
    return 4 ;
  }
  else if (key_sw >= 560) {
    return 2 ;
  }
  else if (key_sw >= 400) {
    return 1 ;
  }
  else {
    return 0 ;
  }
}
void doNothing(void)
{ ;
}

void doInitialState(void)
{
  digitalWrite(VAL1, LOW);
  digitalWrite(VAL2, LOW);
  digitalWrite(VAL3, LOW);
  digitalWrite(MOTOR, LOW);
  digitalWrite(MAINPUMP, LOW);
}
void doMotorOn(void)
{
  digitalWrite(VAL1, LOW);
  digitalWrite(VAL2, LOW);
  digitalWrite(VAL3, HIGH);
  //delay(1000/MODAL);
  digitalWrite(MOTOR, HIGH);
  digitalWrite(MAINPUMP, LOW);
}
void printState(unsigned long watering, unsigned long interval)
{
  lcd.setCursor(9, 0);
  lcd.print(watering / TIMEARGUMENT);
  lcd.setCursor(9, 1);
  lcd.print(interval / TIMEARGUMENT);
}

void watering(unsigned long feed, unsigned long interval, unsigned long duration)
//feed : 공급시간 (분)
//interval : 공급 후 쉬는 시간 (분)
//duration : 총 공급시간 (분)
{
  unsigned long currentDuration; // 현 루프 시간
  unsigned long timeAnchor; //기준시간
  unsigned long loopDuration; // 최상위 루프에서 걸린 시간
  unsigned long loopAnchor ; //최상위 루프에서 기준이 되는 시간

  unsigned long wateringTime = 0;
  unsigned long nWaterDuration = 0;
  unsigned long intervalTime = 0;
  unsigned long nIntervalDuration = 0;

  /* 루프 속에서 모터동작 시간을 계산하기 위한 임시 인수*/
  int loopCount = 0;

  //인수 검사
  if ((feed > duration) || (interval > duration)) return;

  loopAnchor = millis();

  do {
    /*//debugging code
    loopCount += 1;
    Serial.print("LoopCount=");
    Serial.println(loopCount);
    Serial.print("Duration=");
    Serial.println(duration);
    Serial.print("LoopDuration=");
    Serial.println(loopDuration);
    */
    //동작기
    doMotorOn();
    timeAnchor = millis();
    do {
      currentDuration = millis() - timeAnchor;
      if (assignSwitchNumber(SWS) == 1) {
        //
        wateringTime = nWaterDuration + currentDuration;
        printState(wateringTime+100, intervalTime+100);
        doInitialState();
        return;
      }
      wateringTime = nWaterDuration + currentDuration;
      //debuggin code
      Serial.print("WateringTime=");
      Serial.print(wateringTime); Serial.print("   ");
      Serial.print(wateringTime/TIMEARGUMENT);//계산이 안됨
      Serial.print("  currentDuration="); 
      Serial.println(currentDuration);
      //*/
      printState(wateringTime+100, intervalTime+100);
      /*//debugging code
        Serial.print("CurrentDuration=");
        Serial.println(currentDuration);
        Serial.print("feed*TIMEARGUMENT=");
        Serial.println(feed*TIMEARGUMENT);
        //*/
    } while( (currentDuration <= (feed * TIMEARGUMENT)) && ((wateringTime+intervalTime) <= duration*TIMEARGUMENT));
    nWaterDuration = wateringTime;
    currentDuration = 0;
    //debuggin code
    Serial.print("WateringTime=");
    Serial.print(wateringTime); Serial.print("   ");
    Serial.print(wateringTime/TIMEARGUMENT);//계산이 안됨
    Serial.print("  currentDuration="); 
    Serial.println(currentDuration);
    //*/
    
    //휴지기
    doInitialState();
    timeAnchor = millis();
    do {
      currentDuration = millis() - timeAnchor;
      if (assignSwitchNumber(SWS) == 1) {
        intervalTime = nIntervalDuration + currentDuration;
        printState(wateringTime+100, intervalTime+100);
        return;
      }
      intervalTime = nIntervalDuration + currentDuration;
      //debuggin code
      Serial.print("IntervalTime=");
      Serial.print(intervalTime); Serial.print("   ");
      Serial.println(intervalTime/TIMEARGUMENT);
      //*/
      printState(wateringTime+100, intervalTime+100);
    } while ((currentDuration <= interval*TIMEARGUMENT)&&((wateringTime+intervalTime) <= (duration*TIMEARGUMENT)));
    nIntervalDuration = intervalTime;
    currentDuration = 0;

    loopDuration = millis() - loopAnchor;

    /*//debugging code
    Serial.print("LoopDuration=");
    Serial.println(loopDuration);
    Serial.print("Duration*TIMEARGUMENT=");
    Serial.println((unsigned long)duration * TIMEARGUMENT);
    */
    //debuggin code
    Serial.print("LoopDuration=");
    Serial.print(loopDuration); Serial.print("   ");
    Serial.println(loopDuration/TIMEARGUMENT);
    //*/
  } while (loopDuration <= (duration * TIMEARGUMENT));
  printState(wateringTime+100, intervalTime+100);
  doInitialState();
}

void initializeLCD(void)
{
  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print("Watering=       ");
  lcd.setCursor(0, 1);
  lcd.print("Interval=       ");
}

void reserveReservoir(void)
{
  int sw_button; 
  
  digitalWrite(MOTOR, LOW);
  digitalWrite(VAL1, LOW);
  digitalWrite(VAL2, HIGH);
  digitalWrite(VAL3, LOW);
  delay(1000);
  digitalWrite(MAINPUMP, HIGH);
  do {
    sw_button = assignSwitchNumber(SWS);
    Serial.print("Switch Button=");
    Serial.println(sw_button);
  } while (sw_button != 1);
  doInitialState();
}

