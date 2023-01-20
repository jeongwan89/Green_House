//ESP8266과 Arduino통신을 소프트시리얼로 이루어짐
#include <SoftwareSerial.h>
SoftwareSerial ESPSerial(6, 7); //(RX,TX)

#define speed8266 9600
#define HARDWARE_RESET 8 //아두이노를 통한 ESP8266리셋 - 하드웨어 와이어링해야 한다.
#define ssid "FarmMain5G"
#define password "wweerrtt"

// Thingspeak------------------------------------------------------------------
String statusChWriteKey = "SGE75DH6HSE2LRRJ";  // Status Channel id: 908037

// Blynk-----------------------------------------------------------------------
//#include <BlynkSimpleStream.h>
//#define BLYNK_PRINT Serial
/* You should get Auth Token in the Blynk App.
   Go to the Project Settings (nut icon).*/
//char auth[] = "nUuT4bvqJiIDhVWKCm7wBI0u1a7I8UBZ";

// LCD 1602A-------------------------------------------------------------------
#include <LiquidCrystal.h>
const int rs = 16, en = 9, d4 = 10, d5 =11, d6 =12, d7 = 13;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
byte customChar[] = {
  B11000,
  B11000,
  B00110,
  B01001,
  B01000,
  B01000,
  B01001,
  B00110
};
byte HD[] = {
  B10100,
  
  B10100,
  B11101,
  B10101,
  B10101,
  B00011,
  B00101,
  B00111
};

//수분부족분 계산
String HD_VPD;

//DHT--------------------------------------------------------------------------
#include "DHT.h"
#include <stdlib.h>
int pinoDHT = 5;
int tipoDHT =  DHT11;
DHT dht(pinoDHT, tipoDHT); 
int airTemp = 0;
int airHum = 0;

// DS18B20----------------------------------------------------------------------
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 4 // DS18B20 on pin D4 
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
int soilTemp = 0;

// LDR (Light)------------------------------------------------------------------
#define ldrPIN 1      //analog pin 1
int light = 0;
int analight = 0;

// Soil humidity----------------------------------------------------------------
#define soilHumPIN 0  //analog pin 0
int soilHum = 0;
int analHum = 0;

// Variables to be used with Actuators------------------------------------------
boolean pump = 0; 
boolean lamp = 0; 

int spare = 0; //Field 8
//------------------------------------------------------------------------------

String infoString = "AT+CIPSEND=133";
String dataString1;
String dataString2;
String dataString3;

//int state = STATE_IDLE; //프로그램 상태

boolean error;


// Variables to be used with timers----------------------------------------------
unsigned long writeTimingSeconds = 17; // ==> Define Sample time in seconds to send data
unsigned long startWriteTiming = 0;
unsigned long elapsedWriteTime = 0;


//함수 선언부-----------------------------------------------------------------------
boolean sendATcommand(char * command, char * response, int second);
boolean connectWifi(void);
void readSensors(void);
void writeThingSpeak(void);
void EspHardwareReset(void);
void startThingSpeakCmd(void);
String sendThingSpeakGetCmd(String getStr);
String VPD(float temp, float rh);


void setup() {
  pinMode(HARDWARE_RESET,OUTPUT);
  digitalWrite(HARDWARE_RESET, HIGH);

  lcd.begin(16, 2);
  lcd.createChar(0, customChar);
  lcd.createChar(1, HD);
  
  Serial.begin(9600);
  ESPSerial.begin(speed8266); //ESP-01모듈과 UART 시리얼 연결
  DS18B20.begin();
  dht.begin();
  
  EspHardwareReset(); //Reset do Modulo WiFi
  Serial.println("**Ready to start...**");
  delay (1000);

  //ESP-01모듈 리셋------------------------------------------------
  if (!sendATcommand("AT+RST", "ready", 3)) {
    Serial.println("**Error during reset ESP-01...**");
    while(1);
  }
  else {
    Serial.println("**ESP-01 has started...");
  }
  //소프트AP+스테이션 모드로 설정--------------------------------------
  if(!sendATcommand("AT+CWMODE=3", "OK", 3)){
    Serial.println("**Error in changing mode...");
    while(1);
  }
  else {
    Serial.println("**ESP-01 has been set to mode 3...");
  }

  //AP 접속
  if(!connectWifi()){
    Serial.println("**Error in connecting to AP...");
    //while(1);
  }
  else{
    Serial.println("**ESP-01 has been connected to AP...");
  }


//ESP8266접속후 Blynk 실행-------------------------------------------
//  Blynk.begin(auth, ESPSerial);
//-----------------------------------------------------------------  

  lcd.setCursor(0, 0);
  lcd.print("Air:");
  lcd.setCursor(0, 1);
  lcd.print("Soi:");
  
  startWriteTiming = millis(); // starting the "program clock"

}


//퀘리를 이용한 평균값---------------------------------
  const int querryCount = 100;
  int averagesoilHum[querryCount];
//-------------------------------------------------
  
void loop(){

  start: //label 
  error=0;
  
  elapsedWriteTime = millis()-startWriteTiming; 
  readSensors();
  
  if (elapsedWriteTime > (writeTimingSeconds*1000)) 
  {

    writeThingSpeak();
    startWriteTiming = millis();   
  }    
  if (error==1) //Resend if transmission is not completed 
  {       
    Serial.println(" <<<< ERROR >>>>");
    delay (2000);  
    goto start; //go to label "start"
  }  
  while(Serial.available()){
    ESPSerial.write(Serial.read());
  }

  while(ESPSerial.available()){
    Serial.write(ESPSerial.read());
  }

//soilhumidity의 평균값 계산---------------------------------------------
  for(int i = 0; i < querryCount-1 ; i++){
    averagesoilHum[i] = averagesoilHum[i+1];
  }
  averagesoilHum[querryCount-1] = soilHum;

  for(int i = 1; i < querryCount ; i++) {
    averagesoilHum[0] += averagesoilHum[i];
  }
  averagesoilHum[0] = averagesoilHum[0]/querryCount;
//---------------------------------------------------------------------
  Serial.print("Air Temp:");  Serial.println(airTemp);
  Serial.print("Air Humi:");  Serial.println(airHum);
  Serial.print("Soil Tem:");  Serial.println(soilTemp);
  Serial.print("Soil Hum:");  Serial.print(soilHum); Serial.print("\t"); Serial.println(analHum);
  Serial.print("Avr SHum:");  Serial.println(averagesoilHum[0]);
  Serial.print("VPD:"); Serial.println(HD_VPD);
  Serial.print("Light   :");  Serial.print(light); Serial.print("\t"); Serial.println(analight);
  Serial.println("");


//print LCD----------------------------------------------------------
  //for airTemp
  lcd.setCursor(4, 0);
  lcd.print(airTemp);   lcd.write(byte(0));
  //for airHum
  if(airHum < 10) {lcd.setCursor(8, 0);} else {lcd.setCursor(7, 0);}
  lcd.print(airHum);    lcd.print("% ");
  //for soilTemp
  lcd.setCursor(4, 1);
  lcd.print(soilTemp);  lcd.write(byte(0));
  //for soilHum
  if(averagesoilHum[0] < 10) {lcd.setCursor(7,1); lcd.print(" "); lcd.setCursor(8,1);} else {lcd.setCursor(7, 1);}
  lcd.print(averagesoilHum[0]);   lcd.print("% ");
  //for VPD
  lcd.setCursor(11,0); lcd.write(byte(1));
  if(HD_VPD.length()>= 4) {lcd.setCursor(12,0);} else {lcd.setCursor(12,0); lcd.print(" "); lcd.setCursor(13,0);}
  lcd.print(HD_VPD);
  //for light
  lcd.setCursor(12,1);
  if(light < 10) {lcd.print(" ");} lcd.print(light); lcd.setCursor(14,1); lcd.print("w%");
//Blynk operation----------------------------------------------------
//Blynk.run();
//-------------------------------------------------------------------
  
  
}


//함수 정의부분
boolean sendATcommand(char * command, char * response, int second) {
  //최대 대기 시간 설정
  ESPSerial.setTimeout(second*1000);
  //AT명령을 ESP-01모듈로 전달하여 실행
  ESPSerial.println(command);
  Serial.println(command);
  //응답대기
  boolean responseFound = ESPSerial.find(response);
  //최대 대기 시간을 default값인 1초로 재설정
  ESPSerial.setTimeout(1000);

  return responseFound;
}

boolean connectWifi(){
  ESPSerial.setTimeout(10*1000);
  String cmd="AT+CWJAP=\"";
         cmd+=ssid;
         cmd+="\",\"";
         cmd+=password;
         cmd+="\"";
  ESPSerial.println(cmd);       
  Serial.println(cmd);

  boolean responseFound = ESPSerial.find("WIFI CONNECTED");
  ESPSerial.setTimeout(1000);

  return responseFound;
}
/********* Read Sensors value *************/
void readSensors(void)
{
  airTemp = dht.readTemperature();
  airHum = dht.readHumidity();

  DS18B20.requestTemperatures(); 
  soilTemp = DS18B20.getTempCByIndex(0)+ 1.5; // Sensor 0 will capture Soil Temp in Celcius 
                                              // 보정값 1.5도
  analight = analogRead(ldrPIN);            
  light = map(analight, 0, 1023, 0, 100); //LDRDark:0  ==> dark 100%  
  analHum = analogRead(soilHumPIN);
  soilHum = map(analHum, 550, 300, 0, 100); 
  HD_VPD = VPD((float)airTemp, (float)airHum);
}
/********* Connection com TCP com Thingspeak *******/
void writeThingSpeak(void){

  startThingSpeakCmd();

  // preparacao da string GET
  String getStr = "GET /update?api_key=";
  getStr += statusChWriteKey;
//  getStr +="&field1=";
//  getStr += String(pump);
//  getStr +="&field2=";
//  getStr += String(lamp);
  getStr +="&field3=";
  getStr += String(airTemp);
  getStr +="&field4=";
  getStr += String(airHum);
  getStr +="&field5=";
  getStr += String(soilTemp);
  getStr +="&field6=";
  getStr += String(averagesoilHum[0]); //soilHum대신에 평균값을 넣었음
  getStr +="&field7=";
  getStr += String(light);
  getStr +="&field8=";
  getStr += String(HD_VPD);
  getStr += "\r\n\r\n";

  sendThingSpeakGetCmd(getStr); 
}
/********* Reset ESP *************/
void EspHardwareReset(void)
{
  Serial.println("ESP-01 HARDWARE Reseting......."); 
  digitalWrite(HARDWARE_RESET, LOW); 
  delay(500);
  digitalWrite(HARDWARE_RESET, HIGH);
  delay(8000);//읽기 시작에 필요한 시간 
  Serial.println("RESET"); 
}
/********* Start communication with ThingSpeak*************/
void startThingSpeakCmd(void)
{
  ESPSerial.flush();//쓰기를 시작하기 전에 버퍼를 지웁니다
  
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // IP api.thingspeak.com
  cmd += "\",80";
  ESPSerial.println(cmd);
  Serial.print("send ==> Start cmd: ");
  Serial.println(cmd);

  if(ESPSerial.find("Error"))
  {
    Serial.println("AT+CIPSTART error");
    return;
  }
}
/********* send a GET cmd to ThingSpeak *************/
String sendThingSpeakGetCmd(String getStr)
{
  String cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  ESPSerial.println(cmd);
  Serial.print("send ==> lenght cmd: ");
  Serial.println(cmd);

  if(ESPSerial.find((char *)">"))
  {
    ESPSerial.print(getStr);
    Serial.print("send ==> getStr: ");
    Serial.println(getStr);
    delay(500);//이 지연없이 다음 명령에서 사용 중으로 표시됩니다.
    String messageBody = "";
    while (ESPSerial.available()) 
    {
      String line = ESPSerial.readStringUntil('\n');
      if (line.length() == 1) 
      { //actual content starts after empty line (that has length 1)
        messageBody = ESPSerial.readStringUntil('\n');
      }
    }
    Serial.print("MessageBody received: ");
    Serial.println(messageBody);
    return messageBody;
  }
  else
  {
    ESPSerial.println("AT+CIPCLOSE");     // alert user
    Serial.println("ESP8266 CIPSEND ERROR: RESENDING"); //Resend...
    spare = spare + 1;
    error=1;
    return "error";
  } 
}
//VPD 수분부족분 계산 온도와 상대습도를 기준으로------------------------------
String VPD(float temp, float rh){
  float es;
  float HD; //humidity deficience
  String sVPD;

  es = 6.11*pow(10,(7.5*temp/(237.3+temp)));
  HD = es*(1-rh/100);
  sVPD=String(HD+0.5,1);

  return sVPD;
}
