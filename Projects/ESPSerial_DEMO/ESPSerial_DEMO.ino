//ESP8266과 Arduino통신을 소프트시리얼로 이루어짐
#include <SoftwareSerial.h>
SoftwareSerial ESPSerial(6, 7); //(RX,TX)

#define speed8266 9600
#define HARDWARE_RESET 8 //아두이노를 통한 ESP8266리셋 - 하드웨어 와이어링해야 한다.
#define ssid "FarmMain5G"
#define password "wweerrtt"


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  ESPSerial.begin(speed8266); //ESP-01모듈과 UART 시리얼 연결
  EspHardwareReset(); //Reset do Modulo WiFi
  Serial.println("**Ready to start...**");
  delay (1000);
    //소프트AP+스테이션 모드로 설정--------------------------------------
  while(!sendATcommand("AT+CWMODE=3", "OK", 3)){
    Serial.println("**Error in changing mode...");
    delay(500);
  }
    Serial.println("**ESP-01 has been set to mode 3...");

  //AP 접속
  while(!connectWifi()){
    Serial.println("**Error in connecting to AP...");
    delay(500);
  }
  Serial.println("**ESP-01 has been connected to AP...");//원래는 if구문 안에 있다가 while로 뺐다. connectWiFi를 또 부르기 싫어서 그냥 무조건 하는 명령어로 바꾸어 놓았다.
}

void loop() {
  // put your main code here, to run repeatedly:
  while(Serial.available()){
    ESPSerial.write(Serial.read());
  }

  while(ESPSerial.available()){
    Serial.write(ESPSerial.read());
  }
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
