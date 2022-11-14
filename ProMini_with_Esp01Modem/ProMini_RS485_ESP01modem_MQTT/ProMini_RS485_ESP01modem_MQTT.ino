#define _DEBUG_

//ProMini+RS485+HM100+ESP01(At command)+MQTT
//ESP01으로만 하였을 때, RS485에서 data가 들어오지 않았다. 그래서 ProMini에서 softwareSerial을 통하여 통신하니 통신성공하였다.
//이를 기반으로 ProMini의 Serial포트를 debug입출력으로 사용하지 않고 MQTT통신으로 돌리고, debug정보는 MQTT의 Debug주소에 넣어 모니터한다.

#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspServer.h>
#include <WiFiEspUdp.h>
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

// #define _DEBUG_ 인 경우의 핀  할당 선언
#ifdef _DEBUG_
  #define WiFiconnect     11
  #define MQTTconnect     12
  #define Shieldconnect   13
  #define EnteringSetup   14
  #define EnteringLoop    15
#endif

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
WiFiEspClient espClient;
PubSubClient client(espClient);

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

void callback(char* topic, byte* payload, unsigned int length) {
  // Serial.print("Message arrived [");
  // Serial.print(topic);
  // Serial.print("] ");
  for (int i=0;i<length;i++) {
    // Serial.print((char)payload[i]);
  }
  // Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    // Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    #ifdef _DEBUG_
      digitalWrite(MQTTconnect, LOW);
    #endif
    if (client.connect(CLIENT_NAME)) {
      // Serial.println("connected");
      // Once connected, publish an announcement...
      #ifdef _DEBUG_
        digitalWrite(MQTTconnect, HIGH);
      #endif
      client.publish(DEBUG,"Now Connected! hello world");
      // ... and resubscribe
      // client.subscribe("inTopic");
    } else {
      // Serial.print("failed, rc=");
      // Serial.print(client.state());
      // Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup(){
  pinMode(SSerialTxControl, OUTPUT);
  pinMode(Shieldconnect, OUTPUT);
  digitalWrite(Shieldconnect, HIGH);
  #ifdef _DEBUG_
    pinMode(WiFiconnect, OUTPUT);
    pinMode(MQTTconnect, OUTPUT);
    pinMode(Shieldconnect, OUTPUT);
    pinMode(EnteringSetup, OUTPUT);
    pinMode(EnteringLoop, OUTPUT);
    digitalWrite(WiFiconnect, LOW);
    digitalWrite(MQTTconnect, LOW);
    digitalWrite(Shieldconnect, LOW);
    digitalWrite(EnteringSetup, HIGH);
    digitalWrite(EnteringLoop, LOW);
  #endif
  Serial.begin(9600);
  rs485.begin(19200);
  WiFi.init(&Serial);
  CHIP485_SEL_RX;
    // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    // Serial.println("WiFi shield not present");
    // don't continue
    #ifdef _DEBUG_
      digitalWrite(Shieldconnect, LOW);
    #endif
    while (true);
  }
  #ifdef _DEBUG_
    digitalWrite(Shieldconnect, HIGH);
  #endif

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    // Serial.print("Attempting to connect to WPA SSID: ");
    // Serial.println(ssid);
    // Connect to WPA/WPA2 network
    #ifdef _DEBUG_
     digitalWrite(WiFiconnect, LOW);
    #endif
    status = WiFi.begin(ssid, pass);
  }
  #ifdef _DEBUG_
    digitalWrite(WiFiconnect, HIGH);
  #endif
  // you're connected now, so print out the data
  // Serial.println("You're connected to the network");

  //connect to MQTT server
  client.setServer(server, 1883);
  client.setCallback(callback);
  #ifdef _DEBUG_
    digitalWrite(EnteringSetup, LOW);
  #endif
}

void loop(){
  #ifdef _DEBUG_
    digitalWrite(EnteringLoop, HIGH);
  #endif
  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    // Serial.print("Attempting to connect to WPA SSID: ");
    // Serial.println(ssid);
    // Connect to WPA/WPA2 network
    #ifdef _DEBUG_
     digitalWrite(WiFiconnect, LOW);
    #endif
    status = WiFi.begin(ssid, pass);
  }
  #ifdef _DEBUG_
    digitalWrite(WiFiconnect, HIGH);
  #endif

  if(!client.connected()) {
    #ifdef _DEBUG_
      digitalWrite(MQTTconnect, LOW);
    #endif
    reconnect();
  } else {  //client가 wifi와 MQTT에 동시에 연결되어 있을 때, 아래 내용을 publish
    if((millis()-last_refreshed_time) > REFRESH_TIME*1000) {
      DemandData();
      // delay(500);
      ReadData();
      Parsing();
      //Read data and store it to variables hum and temp
      
      // float to char*
      char sTEMP[5];  dtostrf(temp_HM100, 4, 1,sTEMP);
      char sEC[5];    dtostrf(EC, 4, 1, sEC);    
      char sPH[5];    dtostrf(pH, 4, 1, sPH);
      //Print temp and humidity values to serial monitor
      client.publish(MQTT_PUB_HM100_TEMP, sTEMP, true);
      client.publish(MQTT_PUB_EC, sEC, true);
      client.publish(MQTT_PUB_PH, sPH, true);
      
      last_refreshed_time = millis(); 
    }
  client.loop();
  }
  #ifdef _DEBUG_
    digitalWrite(EnteringLoop, LOW);
  #endif
}
