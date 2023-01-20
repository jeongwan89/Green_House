// Pin 배치 선언
const int motor1 = 6;     //Motor1 on/off     (1)
const int motorDir1 = 7;  //Motor1 direction  (1')
const int motor2 = 8;     //Motor2 on/off     (2)
const int motorDir2 = 9;  //Motor2 direction  (2')
const int motor3 = 10;    //Motor3 on/off     (3)
const int motorDir3 = 11; //Motor3 direction  (3')
const int motor4 = 12;    //Motor4 on/off     (4)
const int motorDir4 = 13; //Motor4 direction  (4')
const int controlSelect = 2; //자동 수동 전환버튼
//const int ledPin = 13;    // select the pin for the LED
int mtr[4][2] = { {motor1,motorDir1}, //모터 작동&상태 배열선언
                  {motor2,motorDir2},
                  {motor3,motorDir3},
                  {motor4,motorDir4} };
const int fenetreState[4] = { A5, // 스위치 동작 R-2R회로 연결핀 좌측(1중 비닐)
                              A4, // 스위치 동작 R-2R회로 연결핀 우측(1중 비닐)
                              A7, // 스위치 동작 R-2R회로 연결핀 좌측(2중 비닐)
                              A6};// 스위치 동작 R-2R회로 연결핀 우측(2중 비닐)

int fenetreValue [4]= { 0,  // 1중-좌측
                        0,  // 1중-우측 스위치 상태를 읽은 값
                        0,  // 2중-좌측
                        0 };// 2중-우측 스위치 상태를 읽은 값

String inputString = ""; // a String to hold incoming data through Serial
bool stringComplete = false;

int controlMode = 0; //  자동이었던지 아니었던지를 알아야한다

//함수 선언부분
void InitialMotor(void); //setup에서 모터의 상태를 모두 0으로;
int FindState(int); //스위치의 상태를 조사하고 반환하기
char* Interprete(int); //반환된 스위치 값을 up down 형태로 해석해줌
void Interprete_Act(int*,int); //통합된 모터 채널 드라이브 함수
void ActSerialEvent(void);//Serial에서 입력이 끝나면 함수 실행

void setup() {
  //pin 초기화
  pinMode(controlSelect,INPUT_PULLUP);  //자동&수동전환버튼을 내부 풀업저항을 이용하여 할당
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 2; j++)
      pinMode(mtr[i][j],OUTPUT);
  //Pin의 값을 모두 0으로
  InitialMotor();
  // Serial 시동하기
  Serial.begin(9600);  
  inputString.reserve(200);
}

void loop() {
  // 버튼에서 좌우 측창의 상태를 읽어라:
  int fenetreValue[4];

  if(digitalRead(controlSelect) == HIGH){ //수동모드 pin상태는 스위치 open
    if (controlMode == 1) {
      controlMode = 0;
      for(int i = 0; i < 4; i++)
        for (int j = 0; j < 2; j++)
          digitalWrite(mtr[i][j],LOW);
    }
    for(int i = 0; i < 4; i++)
      fenetreValue[i] = analogRead(fenetreState[i]);
    //측창 스위치의 상태를 여러방법으로 출력
    for(int i = 0; i < 4; i++){
      Serial.print(Interprete(FindState(fenetreValue[i])));
      Serial.print("\t\t");
    }
    Serial.println("");
    for(int i = 0; i < 4; i++)
      Interprete_Act(mtr[i],FindState(fenetreValue[i]));
    // halt the program for <sensorValue> milliseconds:
    delay(100);
  } 
  else {  //자동모드. controlSelect는 LOW. 스위치 접점상태
    if (controlMode == 0) {
      controlMode = 1;
      for(int i = 0; i < 4; i++)
        for (int j = 0; j < 2; j++)
          digitalWrite(mtr[i][j],LOW);
    }
    while(Serial.available()){
      char inChar = (char)Serial.read();
      if(inChar != '\n'){
        inputString += inChar;
      } else {
        stringComplete = true;
        Serial.flush();
      }
    }
    if(stringComplete == true){
      Serial.println(inputString);
      ActSerialEvent(inputString);
      inputString = "";
      stringComplete = false;
    }
  }
}

//함수 정의
void ActSerialEvent(String actString){
  if(actString.equalsIgnoreCase("1 left up")){
    digitalWrite(mtr[0][1],HIGH);
    digitalWrite(mtr[0][0],HIGH);}
  else if(actString.equalsIgnoreCase("1 left down")){
    digitalWrite(mtr[0][1],LOW);
    digitalWrite(mtr[0][0],HIGH);}
  else if(actString.equalsIgnoreCase("1 right up")){
    digitalWrite(mtr[1][1], HIGH);
    digitalWrite(mtr[1][0], HIGH);}
  else if(actString.equalsIgnoreCase("1 right down")){
    digitalWrite(mtr[1][1], LOW);
    digitalWrite(mtr[1][0], HIGH);}
  else if(actString.equalsIgnoreCase("1 all down")||actString.equalsIgnoreCase("1 down")){
    digitalWrite(mtr[0][1], LOW);
    digitalWrite(mtr[0][0], HIGH);
    digitalWrite(mtr[1][1], LOW);
    digitalWrite(mtr[1][0], HIGH);}
  else if(actString.equalsIgnoreCase("1 all up")||actString.equalsIgnoreCase("1 up")){
    digitalWrite(mtr[0][1], HIGH);
    digitalWrite(mtr[0][0], HIGH);
    digitalWrite(mtr[1][1], HIGH);
    digitalWrite(mtr[1][0], HIGH);}     
  else if(actString.equalsIgnoreCase("2 left up")){
    digitalWrite(mtr[2][1],HIGH);
    digitalWrite(mtr[2][0],HIGH);}
  else if(actString.equalsIgnoreCase("2 left down")){
    digitalWrite(mtr[2][1],LOW);
    digitalWrite(mtr[2][0],HIGH);}
  else if(actString.equalsIgnoreCase("2 right up")){
    digitalWrite(mtr[3][1], HIGH);
    digitalWrite(mtr[3][0], HIGH);}
  else if(actString.equalsIgnoreCase("2 right down")){
    digitalWrite(mtr[3][1], LOW);
    digitalWrite(mtr[3][0], HIGH);}
  else if(actString.equalsIgnoreCase("2 all down")||actString.equalsIgnoreCase("2 down")){
    digitalWrite(mtr[2][1], LOW);
    digitalWrite(mtr[2][0], HIGH);
    digitalWrite(mtr[3][1], LOW);
    digitalWrite(mtr[3][0], HIGH);}
  else if(actString.equalsIgnoreCase("2 all up")||actString.equalsIgnoreCase("2 up")){
    digitalWrite(mtr[2][1], HIGH);
    digitalWrite(mtr[2][0], HIGH);
    digitalWrite(mtr[3][1], HIGH);
    digitalWrite(mtr[3][0], HIGH);}
  else if(actString.equalsIgnoreCase("left up")){
    digitalWrite(mtr[0][1], HIGH);
    digitalWrite(mtr[0][0], HIGH);
    digitalWrite(mtr[2][1], HIGH);
    digitalWrite(mtr[2][0], HIGH);}
  else if(actString.equalsIgnoreCase("left down")){
    digitalWrite(mtr[0][1], LOW);
    digitalWrite(mtr[0][0], HIGH);
    digitalWrite(mtr[2][1], LOW);
    digitalWrite(mtr[2][0], HIGH);}
  else if(actString.equalsIgnoreCase("right up")){
    digitalWrite(mtr[1][1], HIGH);
    digitalWrite(mtr[1][0], HIGH);
    digitalWrite(mtr[3][1], HIGH);
    digitalWrite(mtr[3][0], HIGH);}
  else if(actString.equalsIgnoreCase("right down")){
    digitalWrite(mtr[1][1], LOW);
    digitalWrite(mtr[1][0], HIGH);
    digitalWrite(mtr[3][1], LOW);
    digitalWrite(mtr[3][0], HIGH);}
 else if(actString.equalsIgnoreCase("all up")||actString.equalsIgnoreCase("up")){
    digitalWrite(mtr[0][1], HIGH);
    digitalWrite(mtr[0][0], HIGH);
    digitalWrite(mtr[1][1], HIGH);
    digitalWrite(mtr[1][0], HIGH);
    digitalWrite(mtr[2][1], HIGH);
    digitalWrite(mtr[2][0], HIGH);
    digitalWrite(mtr[3][1], HIGH);
    digitalWrite(mtr[3][0], HIGH);}       
  else if(actString.equalsIgnoreCase("all down")||actString.equalsIgnoreCase("down")){
    digitalWrite(mtr[0][1], LOW);
    digitalWrite(mtr[0][0], HIGH);
    digitalWrite(mtr[1][1], LOW);
    digitalWrite(mtr[1][0], HIGH);
    digitalWrite(mtr[2][1], LOW);
    digitalWrite(mtr[2][0], HIGH);
    digitalWrite(mtr[3][1], LOW);
    digitalWrite(mtr[3][0], HIGH);}
  else if(actString.equalsIgnoreCase("stop")){
    for(int i = 0; i < 4; i++)
      for(int j = 0; j < 2; j++)
        digitalWrite(mtr[i][j], LOW);}  
  else {
    //default : do nothing
  }
}
void InitialMotor(void) { //setup에서 Motor의 상태를 모두 정지로 
  //pin 초기화
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 2; j++)
      digitalWrite(mtr[i][j],LOW);
}

int FindState(int crit)  //스위치의 상태를 조사하고 반환하기
{
  if (crit >= 512) {        //down
    return 3;               // 
  } else if (crit >= 128) { //up
    return 2;               //    
  } else
    return 1;               //x 
}

char* Interprete(int state) //반환된 스위치 값을 up down 형태로 해석해줌 
{
  switch (state){
    case 3 : return "down";
    case 2 : return "up  ";
    default : return "x   ";
  }
}
void Interprete_Act(int* ch,int state){ //통합된 모터 채널 드라이브 함수
    switch (state){
      case 3 : //return "down"
            digitalWrite(ch[1],LOW);
            digitalWrite(ch[0],HIGH);
            break;
      case 2 : //return "up  "
            digitalWrite(ch[1], HIGH);
            digitalWrite(ch[0], HIGH);
            break;
      default : //return "x   "
            digitalWrite(ch[1], LOW);
            digitalWrite(ch[0], LOW);
    }
}
