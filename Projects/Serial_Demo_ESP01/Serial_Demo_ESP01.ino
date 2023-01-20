#include <SoftwareSerial.h>
//SoftwareSerial ESPSerial(2, 3); //(RX,TX)
boolean responseFound = true;
#define speed8266 9600
#define HARDWARE_RESET 8 //아두이노를 통한 ESP8266리셋 - 하드웨어 와이어링해야 한다.
#define ssid "FarmMain5G"
#define password "wweerrtt"

void setup() {
  //ESPSerial.begin(speed8266); //ESP-01모듈과 UART 시리얼 연결
  //EspHardwareReset(); //Reset do Modulo WiFi
  Serial.begin(9600);
  //while(!Serial){;}
  Serial1.begin(9600);
  //while(!Serial1){;}
}

void loop() {
  while(Serial.available()){
    Serial1.write(Serial.read());
    //ESPSerial.write(Serial.read());
  }

  while(Serial1.available()){
  //while(ESPSerial.available()){
    Serial.write(Serial1.read());
//    Serial.write(ESPSerial.read());
  }
}

boolean sendATcommand(char * command, char * response, int second) {
  //최대 대기 시간 설정
//  ESPSerial.setTimeout(second*1000);
  //AT명령을 ESP-01모듈로 전달하여 실행
//  ESPSerial.println(command);
  Serial.println(command);
  //응답대기
//  boolean responseFound = ESPSerial.find(response);
  //최대 대기 시간을 default값인 1초로 재설정
//  ESPSerial.setTimeout(1000);

  return responseFound;
}

boolean connectWifi(){
//  ESPSerial.setTimeout(10*1000);
  String cmd="AT+CWJAP=\"";
         cmd+=ssid;
         cmd+="\",\"";
         cmd+=password;
         cmd+="\"";
//  ESPSerial.println(cmd);       
  Serial.println(cmd);

//  boolean responseFound = ESPSerial.find("WIFI CONNECTED");
//  ESPSerial.setTimeout(1000);

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
