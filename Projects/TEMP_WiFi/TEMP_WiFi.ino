/*
    ---------------------------------------------------------------------------------------
    온실 모니터 시스템이다.
    MCU은 arduino pro mini이고 wifi통신은 ESP-01모뎀이다. ESP-01은 AT 커멘드로 입출력한다.
    4동이 다 똑같을 수는 없으므로 한 소스 위에 #define으로 각 동의 특성을 재정의한다.
    사용하다 보니 보정값이 필요한데, 온실 동에 따른 센서의 보정값을 #define 구문에 정의하도록 하자.
    ---------------------------------------------------------------------------------------
*/

#define GH2
#ifdef GH1
    #define WRITEKEY    "TT36OUSUL4MCBWZG"
    #define CORRECTION_AIRTEMP  0
    #define CORRECTION_AIRHUM   0
    #define CORRECTION_SOILTEMP 0
#endif

#ifdef GH2
    #define WRITEKEY    "SGE75DH6HSE2LRRJ"
    #define CORRECTION_AIRTEMP  0
    #define CORRECTION_AIRHUM   0
    #define CORRECTION_SOILTEMP 0.5
#endif

#ifdef GH3
    #define WRITEKEY    "1H0JQ4NC5IP09387"
    #define CORRECTION_AIRTEMP  0
    #define CORRECTION_AIRHUM   0
    #define CORRECTION_SOILTEMP -1
#endif

#ifdef GH4
    #define WRITEKEY    "LSTSCT2AUIMICLQT"
    #define CORRECTION_AIRTEMP  0
    #define CORRECTION_AIRHUM   0
    #define CORRECTION_SOILTEMP 1
#endif

#ifdef GH2_MAC
    #define WRITEKEY    "YCHX03BBCU6KI8MO"
    #define CORRECTION_AIRTEMP  0
    #define CORRECTION_AIRHUM   0
    #define CORRECTION_SOILTEMP 0
#endif
//ESP8266과 Arduino통신을 소프트시리얼로 이루어짐------------------------ESP8266과 Arduino통신
#include <SoftwareSerial.h>
SoftwareSerial ESPSerial(6, 7); //(RX,TX)

//CO2 센서를 위한 코드삽입 MH-Z19B-------------------------------------------------CO2 센서
//MHZ19라이브러리가 너무 광범위하고 난삽하게 작성되어서 기본만 여기에서 구현하도록 한다.
//유저는 PWM을 읽는 함수를 짜거나
//Analog input을 이용하여 유저 함수를 만드는 것이 더욱 유리하다.
/*#include <MHZ19pwmppm.h>
MHZ19 *mhz19_pwm = new MHZ19(CO2_pwmpin);
mhz19_pwm->setAutoCalibration(false);
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

// Thingspeak-------------------------------------------------------------------Thingspeak
String statusChWriteKey = WRITEKEY;  // Status Channel id: 846408

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

//함수 정의부분-----------------------------------------------------------

unsigned int getPpmPWM(int _PIN){
    unsigned long th, tl;
    unsigned int ppm=0;
    
    th = (pulseIn(_PIN, HIGH,2500000)/1000); 
    ppm = 2000*(th-2)/1000;
    return ppm;
}

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
    airTemp = dht.readTemperature() + CORRECTION_AIRTEMP;
    airHum = dht.readHumidity()+ CORRECTION_AIRHUM;

    DS18B20.requestTemperatures(); 
    soilTemp = DS18B20.getTempCByIndex(0) + CORRECTION_SOILTEMP ; // Sensor 0 will capture Soil Temp in Celcius 
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
    if(analHum > 850) 
        soilHum = map(analHum, 1000, 850, 0, 10);
    else if (analHum > 525)
        soilHum = map(analHum, 850, 525, 10, 90);
    else
        soilHum = map(analHum, 525, 450, 90, 100);

    
    HD_VPD = VPD((float)airTemp, (float)airHum);
    }


    /********* Connection com TCP com Thingspeak *******/
    void writeThingSpeak(void)
    {
    startThingSpeakCmd();

    // preparacao da string GET
    String getStr = "GET /update?api_key=";
    getStr += statusChWriteKey;
    //  getStr +="&field1=";
    //  getStr += String(pump);
    getStr +="&field2=";
    getStr += String(CO2pwm);
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

    //debug
    //Serial.print("debug:"); Serial.print(getStr);
    //debug
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
    //debug
    //  Serial.print("debug:");   Serial.println(getStr);
    //debug
    ESPSerial.println(cmd);
    Serial.print("send ==> lenght cmd: ");
    Serial.println(cmd);

    if(ESPSerial.find(">")){
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
        Serial.println(getStr);
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

void setup() {
    pinMode(HARDWARE_RESET,OUTPUT);
    digitalWrite(HARDWARE_RESET, HIGH); //출력핀 풀업저항을 달 필요가 없음
    
    pinMode(PWM_PIN, INPUT);
    digitalWrite(PWM_PIN, HIGH);
    
    lcd.begin(16, 2);
    lcd.createChar(0, customChar);
    lcd.createChar(1, HD);
    
    Serial.begin(115200);
    // 센서 초기화부분-------------------------------------------
    ESPSerial.begin(speed8266); //ESP-01모듈과 UART 시리얼 연결
    DS18B20.begin();
    dht.begin();
    
    lcd.setCursor(0, 0);
    lcd.print("Resetting ESP01");
    lcd.setCursor(0, 1);
    lcd.print("MH-Z19 warming:");
    delay(3000);
    lcd.setCursor(7,1);// lcd.print(mhz19_pwm->getStatus()); lcd.print("        ");
    
    EspHardwareReset(); //Reset do Modulo WiFi
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("**Ready to start...**");
    delay (1000);

    //ESP-01모듈 리셋------------------------------------------------
    while (!sendATcommand("AT+RST", "ready", 3)) {
        Serial.println("**Error during reset ESP-01...**");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Error during reset ESP");
  }
  //초기화가 넘어가면
  Serial.println("**ESP-01 has started...");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ESP01 has started");

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
    while(connectWifi());
  }
  else{
    Serial.println("**ESP-01 has been connected to AP...");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("ESP01 connected to AP...");
  }


/*ESP8266접속후 Blynk 실행-------------------------------------------
//  Blynk.begin(auth, ESPSerial);
//----------------------------------------------------------------- */

    lcd.setCursor(0, 0);
    lcd.print("Air:");
    lcd.setCursor(0, 1);
    lcd.print("Soi:");
    
    startWriteTiming = millis(); // starting the "program clock"
    lastReadTimingCO2 = millis();
}

void loop(){
    
    start: //label 
    error=0;
    
    elapsedWriteTime = millis()-startWriteTiming; 
    elapsedReadingTimeCO2 = millis()-lastReadTimingCO2;
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

    //CO2 Sensor Reading during CO2interval
    if(elapsedReadingTimeCO2 > (CO2interval*1000)){
        // CO2 센서를 더이상 멤버 함수로 쓰지 않고 외부 함수로 돌려서 쓰도록 함
        // 이유. 아날로그 핀처럼 그냥 pulseIn함수로 유저 프로그램 위에서 쓰는 것이 더 가벼움
        // CO2ppm = mhz19_pwm->getPpmPwm();
        CO2pwm = getPpmPWM(PWM_PIN); 
        lastReadTimingCO2=millis();
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

    
    //---------------------------------------------------------------------
    Serial.print("Air Temp:");  Serial.print(airTemp); Serial.print("\t");
    Serial.print("Air Humi:");  Serial.print(airHum);Serial.print("\t");
    Serial.print("Soil Tem:");  Serial.print(soilTemp);Serial.print("\t");
    Serial.print("Soil Hum:");  Serial.print(soilHum); Serial.print("\t"); Serial.print(analHum);Serial.print("\t");
    Serial.print("Avr SHum:");  Serial.print(averagesoilHum[0]);Serial.print("\t");
    Serial.print("VPD:");       Serial.print(HD_VPD);Serial.print("\t");
    Serial.print("Light:");     Serial.print(light); Serial.print("\t"); Serial.print(analight);Serial.print("\t");
    Serial.print("CO2:");       Serial.print(CO2pwm);
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
    //for light and CO2 Alternative
    if((millis()/5000)%2 == 0){
        lcd.setCursor(10,1); lcd.print("  ");
        lcd.setCursor(12,1);
        if(light < 10) {lcd.print(" ");} lcd.print(light); lcd.setCursor(14,1); lcd.print("w%");
    } else{
        lcd.setCursor(10,1);
        lcd.print(CO2pwm); lcd.setCursor(14,1); lcd.print("ppm");
    }
} 
//End of Loop()