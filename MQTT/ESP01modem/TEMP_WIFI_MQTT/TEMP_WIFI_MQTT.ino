//ESP8266과 Arduino통신을 소프트시리얼로 이루어짐--------------------------
#include <SoftwareSerial.h>
SoftwareSerial ESPSerial(6, 7); //(RX,TX)

//ESP-01 선언부--------------------------------------------------------
#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#define speed8266 9600
#define HARDWARE_RESET 8

//MQTT 이용 데이터 전송
#include <PubSubClient.h>
WiFiEspClient esp8266Client;
PubSubClient mqtt_client(esp8266Client);
/*
//CO2 센서를 위한 코드삽입 MH-Z19B---------------------------------------
//MHZ19라이브러리가 너무 광범위하고 난삽하게 작성되어서 기본만 여기에서 구현하도록 한다.
//유저는 PWM을 읽는 함수를 짜거나
//Analog input을 이용하여 유저 함수를 만드는 것이 더욱 유리하다.
#include <MHZ19pwmppm.h>
MHZ19 *mhz19_pwm = new MHZ19(CO2_pwmpin);

#define CO2_pwmpin 3
#define CO2interval 5 //5초만에 새로운 값을 읽도록 한다. 5초 동안은 그대로 이전 값을 유지한다.
#define PWM_PIN 3
#define CO2_ANALOG_PIN A3 //아날로그 핀 3번으로 CO2 데이터를 읽는다.
int CO2ppm = 0;     //CO2 센서를 위한 변수
unsigned int CO2pwm=0; 
*/
byte mac[6];

/*// Blynk----------------------------------------------------------------------
//#include <BlynkSimpleStream.h>
//#define BLYNK_PRINT Serial
/* You should get Auth Token in the Blynk App.
   Go to the Project Settings (nut icon).
//char auth[] = "nUuT4bvqJiIDhVWKCm7wBI0u1a7I8UBZ";
*/

// LCD 1602A-------------------------------------------------------------------
#include <LiquidCrystal.h>
#define rs  16
#define en  9
#define d4  10
#define d5  11
#define d6  12
#define d7  13
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
byte customChar[] = {
  B11000,  B11000,  B00110,  B01001,  B01000,  B01000,  B01001,  B00110
};
byte HD[] = {
  B10100,  B10100,  B11101,  B10101,  B10101,  B00011,  B00101,  B00111
};

//수분부족분 계산
String HD_VPD;

//DHT--------------------------------------------------------------------------
#include <DHT.h>
#include <stdlib.h>
#define pinoDHT  5
int tipoDHT =  DHT22;
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
unsigned long writeTimingSeconds = 3; // ==> Define Sample time in seconds to send data
unsigned long startWriteTiming = 0;
unsigned long elapsedWriteTime = 0;


//함수 선언부-----------------------------------------------------------------------
String VPD(float temp, float rh);


//퀘리를 이용한 평균값---------------------------------
//이곳의 위치는 전역변수(global variables)-------------
const int querryCount = 10;
int averagesoilHum[querryCount];

/*
//CO2센서를 CO2interval 단위로 읽기위한 변수 설정----------------
unsigned long lastReadTimingCO2 = 0;
*/
unsigned int getPpmPWM(int _PIN){
  unsigned long th, tl;
  unsigned int ppm=0;
  
  th = (pulseIn(_PIN, HIGH,3500000)/1000); 
  ppm = 2000*(th-2)/1000;
  return ppm;
}

#define ssid "FarmMain5G"
#define password "wweerrtt"

#define WIFI_SSID   "KT_GiGA_08C8"
#define WIFI_PW     "cf05zc0562"
#define MQTT_BROKER_ADDR  "172.30.1.28"
#define MQTT_BROKER_PORT  1883
#define MQTT_ID   "farmmain5g"
#define MQTT_PW   "eerrtt"
#define MQTT_USER   "NRSRY"
int status = WL_IDLE_STATUS;   // the Wifi radio's status
char publish_msg[16];

//온도 습도 보정값 ------------------------------------------------------
#define calib_soilTemp 0
#define calib_airTemp 0
#define calib_airHum 0

//토양센서 보정값
//2cm 물 깊이에서의 값 humA2
//7cm 물 깊이에서의 값 humA7
#define humA2 850
#define humA7 525

//Test situation
#define DURATION 1000

/********* Read Sensors value *************/

void readSensors(void)
{ 
  airTemp = dht.readTemperature() + calib_airTemp;
  airHum = dht.readHumidity()+ calib_airHum;

  DS18B20.requestTemperatures(); 
  soilTemp = DS18B20.getTempCByIndex(0) + calib_soilTemp ; // Sensor 0 will capture Soil Temp in Celcius 
                                              // 보정값 1.5도
  analight = analogRead(ldrPIN);            
  light = map(analight, 0, 1023, 0, 100); //LDRDark:0  ==> dark 100%  

  
  /*
  토양수분을을 비선형으로 하기로 하였으니 아래 선형 코드는 무시
  analHum = analogRead(soilHumPIN);
  soilHum = map(analHum, 660, 300, 0, 100);  //건조할때 660, 물속일때 300
  */
  
  //토양수분율의 비선형 조사표. 이 결정은 센서버저 1.2 기준으로 작성함
  // analHum == 1000, soilHum=0, ~ analHum == 850, soilHum = 10
  // analHum == 850, soilHum=10, ~analHum == 525, soilHum = 90
  // analHum == 525, soilHum=90, ~analHum == 450, soilHum = 100
  analHum = analogRead(soilHumPIN);
  if(analHum > humA2) 
    soilHum = map(analHum, 1000, humA2, 0, 10);
  else if (analHum > humA7)
    soilHum = map(analHum, humA2, humA7, 10, 90);
  else
    soilHum = map(analHum, humA7, 450, 90, 100);

  
  HD_VPD = VPD((float)airTemp, (float)airHum);
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

void init_MQTT_ESP01()
{
  WiFi.init(&ESPSerial);
  // check for the presence of the shield
    if (WiFi.status() == WL_NO_SHIELD) {
        Serial.println("WiFi shield not present");
    // don't continue
    while (true);
    }
  // attempt to connect to WiFi network
    while ( status != WL_CONNECTED) {
       Serial.print("Attempting to connect to WPA SSID: ");
       Serial.println(WIFI_SSID);
       // Connect to WPA/WPA2 network
       status = WiFi.begin(WIFI_SSID, WIFI_PW);
    }
  // you're connected now, so print out the data
    Serial.println("You're connected to the network"); 
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP()); 
    Serial.print("MAC address = ");
    // Display MAC Address
        WiFi.macAddress(mac);
        Serial.print(mac[5],HEX);
        Serial.print(":");
        Serial.print(mac[4],HEX);
        Serial.print(":");
        Serial.print(mac[3],HEX);
        Serial.print(":");
        Serial.print(mac[2],HEX);
        Serial.print(":");
        Serial.print(mac[1],HEX);
        Serial.print(":");
        Serial.println(mac[0],HEX);

        mqtt_client.setServer(MQTT_BROKER_ADDR, MQTT_BROKER_PORT);
}

void establish_mqtt_connection(){
  if(mqtt_client.connected()) 
    return;
  while(!mqtt_client.connected()){
    Serial.println("Try to connect MQTT Broker");
    if(mqtt_client.connect(MQTT_ID, MQTT_USER, MQTT_PW)){
      Serial.println("Connected");
    } else {
      Serial.print("failed, rc=");
      Serial.println(mqtt_client.state());
      delay(2000);
    }
  }
}

void setup() {
//  pinMode(HARDWARE_RESET,OUTPUT); /*ESP-01 하드웨어리셋 코드*/
//  digitalWrite(HARDWARE_RESET, HIGH);

  lcd.begin(16, 2);
  lcd.createChar(0, customChar);
  lcd.createChar(1, HD);
  
  Serial.begin(115200);
  
// 센서 초기화부분-------------------------------------------
  DS18B20.begin();
  dht.begin();
  //mhz19_pwm->setAutoCalibration(false);
  
  lcd.setCursor(0, 0);
  lcd.print("Resetting ESP01");
  lcd.setCursor(0, 1);
  lcd.print("MH-Z19 warming:");
  delay(3000);
  lcd.setCursor(7,1); //lcd.print(mhz19_pwm->getStatus()); lcd.print("        ");
  
  
//2021.8. ESP01 소켓사용으로 인하여 하드웨어 리셋 불가능. AT 커멘드로 대체 
//  EspHardwareReset(); //Reset do Modulo WiFi
  Serial.println("**Ready to start...**");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("**Ready to start...**");
  delay (1000);

//MQTT 초기화부분----------------------------------------------------
    ESPSerial.begin(speed8266); //ESP-01모듈과 UART 시리얼 연결
    init_MQTT_ESP01();
//-----------------------------------------------------------------

  lcd.setCursor(0, 0);
  lcd.print("Air:");
  lcd.setCursor(0, 1);
  lcd.print("Bdg:");
  
  startWriteTiming = millis(); // starting the "program clock"

}

void loop(){
  
  unsigned long elapsedReadingTimeCO2;
  
  establish_mqtt_connection();
  mqtt_client.loop();
  
  elapsedWriteTime = millis()-startWriteTiming; 
//  elapsedReadingTimeCO2 = millis()-lastReadTimingCO2;
  readSensors();

/*
  //CO2 Sensor Reading during CO2interval
  if(elapsedReadingTimeCO2 > (CO2interval*1000)){
    // CO2 센서를 더이상 멤버 함수로 쓰지 않고 외부 함수로 돌려서 쓰도록 함
    // 이유. 아날로그 핀처럼 그냥 pulseIn함수로 유저 프로그램 위에서 쓰는 것이 더 가벼움
    // CO2ppm = mhz19_pwm->getPpmPwm();
    CO2pwm = getPpmPWM(PWM_PIN); 
    lastReadTimingCO2;
  }
*/

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

  
  
  Serial.print("Air Temp:");  Serial.print(airTemp);
  Serial.print("\tAir Humi:");  Serial.print(airHum);
  Serial.print("\tSoil Tem:");  Serial.print(soilTemp);
  Serial.print("\tSoil Hum:");  Serial.print(soilHum); Serial.print("\t"); Serial.print(analHum);
  Serial.print("\tAvr SHum:");  Serial.print(averagesoilHum[0]);
  Serial.print("\tVPD:"); Serial.print(HD_VPD);
  Serial.print("\tLight   :");  Serial.print(light); Serial.print("\t"); Serial.print(analight);
//  Serial.print("\tCO2:"); Serial.print(CO2pwm); Serial.print("\tanalog:"); Serial.print(analogRead(CO2_ANALOG_PIN)); 
//  Serial.print("\tConversion:");  Serial.print((analogRead(CO2_ANALOG_PIN)-82)*2000/328); 
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
    lcd.setCursor(10,1); lcd.print("  ");
    lcd.setCursor(12,1);
    if(light < 10) {lcd.print(" ");} lcd.print(light); lcd.setCursor(14,1); lcd.print("w%");
/*
  //for light and CO2 Alternative
  if((millis()/5000)%2 == 0){
    lcd.setCursor(10,1); lcd.print("  ");
    lcd.setCursor(12,1);
    if(light < 10) {lcd.print(" ");} lcd.print(light); lcd.setCursor(14,1); lcd.print("w%");
  } else{
    lcd.setCursor(10,1); lcd.print("       ");lcd.setCursor(10,1);
    lcd.print(CO2pwm); lcd.print("ppm");
  }
*/  
  if (elapsedWriteTime > (writeTimingSeconds*DURATION)) 
  {
    sprintf(publish_msg, "%d", airTemp);
    mqtt_client.publish("nursery/airTemp", publish_msg);

    sprintf(publish_msg, "%d", airHum);
    mqtt_client.publish("nursery/airHum", publish_msg);

    sprintf(publish_msg, "%d", soilTemp);
    mqtt_client.publish("nursery/soilTemp", publish_msg);

    sprintf(publish_msg, "%d", averagesoilHum[0]);
    mqtt_client.publish("nursery/soilHum", publish_msg);

    sprintf(publish_msg, "%s", HD_VPD.c_str());
    mqtt_client.publish("nursery/hd", publish_msg);

    sprintf(publish_msg, "%d", light);
    mqtt_client.publish("nursery/light", publish_msg);
    
    startWriteTiming = millis();   
  }    
}