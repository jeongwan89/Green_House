
#define _DEBUG_

#include <ArduinoWiFiServer.h>
#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiGratuitous.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiUdp.h>

//ProMini+RS485+HM100+ESP01(At command)+MQTT
//ESP01으로만 하였을 때, RS485에서 data가 들어오지 않았다. 그래서 ProMini에서 softwareSerial을 통하여 통신하니 통신성공하였다.
//이를 기반으로 ProMini의 Serial포트를 debug입출력으로 사용하지 않고 MQTT통신으로 돌리고, debug정보는 MQTT의 Debug주소에 넣어 모니터한다.


#include <PubSubClient.h>

#include <SoftwareSerial.h>

#define SSerialTxControl 10

#define CHIP485_SEL_TX digitalWrite(SSerialTxControl,HIGH)  //Transmission
#define CHIP485_SEL_RX digitalWrite(SSerialTxControl,LOW)   //Receive

#define ESP01_DHT22_RS485_MQTT_02

#ifdef ESP01_DHT22_RS485_MQTT_01
#define CLIENT_NAME         "Green_House_HM100_Temp_RH_Monitor_01"
#define MQTT_PUB_PH         "Sensor/GH1/Rear/pH"
#define MQTT_PUB_HM100_TEMP "Sensor/GH1/Rear/HM_TEMP"
#define MQTT_PUB_EC         "Sensor/GH1/Rear/EC"
#define MQTT_PUB_TEMP       "Sensor/GH1/Rear/Temp"        //GH1 = Green House (온실) 1동
#define MQTT_PUB_HUM        "Sensor/GH1/Rear/Hum"
#define SENSOR_STATUS       "Sensor/GH1/Rear/Stat"
#define TEMP_CAL  0
#define HUM_CAL   0
#endif

#ifdef ESP01_DHT22_RS485_MQTT_02
#define CLIENT_NAME         "Green_House_HM100_Temp_RH_Monitor_02"
#define MQTT_PUB_PH         "Sensor/GH2/Rear/pH"
#define MQTT_PUB_HM100_TEMP "Sensor/GH2/Rear/HM_TEMP"
#define MQTT_PUB_EC         "Sensor/GH2/Rear/EC"
#define MQTT_PUB_TEMP       "Sensor/GH2/Rear/Temp"
#define MQTT_PUB_HUM        "Sensor/GH2/Rear/Hum"
#define SENSOR_STATUS       "Sensor/GH2/Rear/Stat"
#define DEBUG               "Sensor/GH2/Rear/Debug"
#define TEMP_CAL  0
#define HUM_CAL   0
#endif

#ifdef ESP01_DHT22_RS485_MQTT_03
#define CLIENT_NAME         "Green_House_HM100_Temp_RH_Monitor_03"
#define MQTT_PUB_PH         "Sensor/GH3/Rear/pH"
#define MQTT_PUB_HM100_TEMP "Sensor/GH3/Rear/HM_TEMP"
#define MQTT_PUB_EC         "Sensor/GH3/Rear/EC"
#define MQTT_PUB_TEMP       "Sensor/GH3/Rear/Temp"
#define MQTT_PUB_HUM        "Sensor/GH3/Rear/Hum"
#define SENSOR_STATUS       "Sensor/GH3/Rear/Stat"
#define TEMP_CAL  0
#define HUM_CAL   0
#endif

#ifdef ESP01_DHT22_RS485_MQTT_04
#define CLIENT_NAME         "Green_House_HM100_Temp_RH_Monitor_04"
#define MQTT_PUB_PH         "Sensor/GH4/Rear/pH"
#define MQTT_PUB_HM100_TEMP "Sensor/GH4/Rear/HM_TEMP"
#define MQTT_PUB_EC         "Sensor/GH4/Rear/EC"
#define MQTT_PUB_TEMP       "Sensor/GH4/Rear/Temp"
#define MQTT_PUB_HUM        "Sensor/GH4/Rear/Hum"
#define SENSOR_STATUS       "Sensor/GH4/Rear/Stat"
#define TEMP_CAL  0
#define HUM_CAL   0
#endif

#ifdef ESP01_DHT22_RS485_MQTT_05
#define CLIENT_NAME         "NR_House_HM100_Temp_RH_Moitor_01"
#define MQTT_PUB_PH         "Sensor/NR1/Rear/pH"
#define MQTT_PUB_HM100_TEMP "Sensor/NR1/Rear/HM_TEMP"
#define MQTT_PUB_EC         "Sensor/NR1/Rear/EC"
#define MQTT_PUB_TEMP       "Sensor/NR1/Rear/Temp"          //NR1 = Nursery Green House (육묘장) 1동
#define MQTT_PUB_HUM        "Sensor/NR1/Rear/Hum"
#define SENSOR_STATUS       "Sensor/NR1/Rear/Stat"
#define TEMP_CAL  0
#define HUM_CAL   0
#endif

#define REFRESH_TIME 10  //sec REFRESH_TIME 마다 온도/습도를 읽어서 MQTT에 올리기 위한 인터벌 시간

// Variables
// float hum;  //Stores humidity value from DHT22
// float temp; //Stores temperature value from DHT22
// volatile boolean state = true;
float EC;
float pH;
float temp_HM100;
long lastReconnectAttempt = 0;

//loop()에서 시간 간격당 실행시키기 위한 변수선언
unsigned long last_refreshed_time;

SoftwareSerial rs485(6,7); //RX-RO TX-DI
unsigned char Data[16];
byte  requestData[9]={
        0x01,   //Device ID
        0x03,   //Function
        0x00,   //Address high byte
        0x68,   //Address low byte
        0x00,   //Data high byte
        0x03,   //Data low byte
        0x84,   //CRC low byte
        0x17    //CRC high byte
      };     

// WiFiESP.h에서 걸리는 선언부분
// ESP01 modem을 위한 선언
IPAddress server(192, 168, 0, 24);
char ssid[] = "FarmMain5G";       // your network SSID (name)
char pass[] = "wweerrtt";         // your network password
int status = WL_IDLE_STATUS;      // the Wifi radio's status

// Initialize the Ethernet client object

//PubSubClient client(espClient);

void DemandData(void)
{
  char mP[5];
  CHIP485_SEL_TX;

  //이후에 SoftwareSerial rs485에 byte requestData 써 넣는 루틴
  //rs485.write(requestData,8);
  //한 바이트씩 보내기
  for (int i=0; i<8; i++){
   rs485.write(requestData[i]);
  }

  // Serial.print("Demand Data to HM-100 sending:");
  // for(int i=0; i<8; i++){
  //   sprintf(mP, "0x%02x ", requestData[i]);
  //   Serial.print(mP);
  // }
  // Serial.println();

  CHIP485_SEL_RX;
}

int ReadData(void){
  int index =0;
  char mP[5];
  CHIP485_SEL_RX;

  index = rs485.available();
  // if(index >0){
  //   // Serial.print("Received Data:");
  //   for(int i=0; i<index; i++){
  //      Data[i] = rs485.read();
  //      sprintf(mP, "0x%02x ", Data[i]);
  //      Serial.print(mP);
  //   }
    // Serial.print("\t"); Serial.println(index);
  // }
  CHIP485_SEL_RX;
  return index;
}

void Parsing(void){
  EC = (float)((int)Data[3]*256+(int)Data[4])/100;
  pH = (float)((int)Data[5]*256+(int)Data[6])/100;
  temp_HM100 = (float)((int)Data[7]*256+(int)Data[8])/10;

  for(int i=0; i<16; i++){
      Data[i] = 0x00;
  }
  //참고로 pH, temp, EC는 hm-100의 단위 select에 따라 달라진다.
  // Serial.print("pH = "); Serial.print(pH); Serial.print('\t');
  // Serial.print("tp = "); Serial.print(temp_HM100); Serial.print('\t');
  // Serial.print("EC = "); Serial.println(EC);
}

void setup(){
  pinMode(SSerialTxControl, OUTPUT);
  Serial.begin(115200);
  rs485.begin(19200);
  CHIP485_SEL_RX;
}

void loop(){
  DemandData();
  delay(1000);
  ReadData();
  Parsing();
  delay(1000);
}
