//ESP8266과 Arduino통신을 소프트시리얼로 이루어짐------------------------ESP8266과 Arduino통신
#include <SoftwareSerial.h>
SoftwareSerial ESPSerial(6, 7); //(RX,TX)

//CO2 센서를 위한 코드삽입 MH-Z19B-------------------------------------------------CO2 센서
//MHZ19라이브러리가 너무 광범위하고 난삽하게 작성되어서 기본만 여기에서 구현하도록 한다.
//유저는 PWM을 읽는 함수를 짜거나
//Analog input을 이용하여 유저 함수를 만드는 것이 더욱 유리하다.
/*#include <MHZ19pwmppm.h>
MHZ19 *mhz19_pwm = new MHZ19(CO2_pwmpin);
*/
#define CO2interval 5 //5초만에 새로운 값을 읽도록 한다. 5초 동안은 그대로 이전 값을 유지한다.
#define PWM_PIN 3
//int CO2ppm = 0;     //CO2 센서를 위한 변수
unsigned int CO2pwm=0; 

//ESP-01 선언부---------------------------------------------------------------ESP-01 선언부
#define speed8266 9600
#define HARDWARE_RESET 8 //아두이노를 통한 ESP8266리셋 - 하드웨어 와이어링해야 한다. 하지만 2021년 8월 버전은 ESP01소켓으로 인하여 리셋 불가능
#define ssid "FarmMain5G"
#define password "wweerrtt"

//온도 습도 보정값 ------------------------------------------------------------온도 습도 보정값
#define calib_soilTemp 0
#define calib_airTemp 0
#define calib_airHum 0


// Thingspeak-------------------------------------------------------------------Thingspeak
String statusChWriteKey = "TT36OUSUL4MCBWZG";  // Status Channel id: 846408

/*// Blynk-----------------------------------------------------------------------
//#include <BlynkSimpleStream.h>
//#define BLYNK_PRINT Serial
/* You should get Auth Token in the Blynk App.
   Go to the Project Settings (nut icon).
//char auth[] = "nUuT4bvqJiIDhVWKCm7wBI0u1a7I8UBZ";
*/

// LCD 1602A----------------------------------------------------------------------LCD 1602A
#include <LiquidCrystal.h>
const int rs = 16, en = 9, d4 = 10, d5 =11, d6 =12, d7 = 13;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
byte customChar[] = {
  B11000,  B11000,  B00110,  B01001,  B01000,  B01000,  B01001,  B00110
};
byte HD[] = {
  B10100,  B10100,  B11101,  B10101,  B10101,  B00011,  B00101,  B00111
};

//수분부족분 계산
String HD_VPD;

//DHT------------------------------------------------------------------------------------DHT
#include <DHT.h>
#include <stdlib.h>
int pinoDHT = 5;
int tipoDHT =  DHT22;
DHT dht(pinoDHT, tipoDHT); 
int airTemp = 0;
int airHum = 0;

// DS18B20----------------------------------------------------------------------------DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 4 // DS18B20 on pin D4 
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
int soilTemp = 0;

// LDR (Light)----------------------------------------------------------------------LDR (Light
#define ldrPIN 1      //analog pin 1
int light = 0;
int analight = 0;

// Soil humidity------------------------------------------------------------------Soil humidity
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


//함수 선언부-----------------------------------------------------------------------함수 선언부
boolean sendATcommand(char * command, char * response, int second);
boolean connectWifi(void);
void readSensors(void);
void writeThingSpeak(void);
//void EspHardwareReset(void);
void startThingSpeakCmd(void);
String sendThingSpeakGetCmd(String getStr);
String VPD(float temp, float rh);


//퀘리를 이용한 평균값---------------------------------
//이곳의 위치는 전역변수(global variables)-------------
const int querryCount = 10;
int averagesoilHum[querryCount];

//CO2센서를 CO2interval 단위로 읽기위한 변수 설정----------------
unsigned long lastReadTimingCO2 = 0;
unsigned long elapsedReadingTimeCO2 =0;
