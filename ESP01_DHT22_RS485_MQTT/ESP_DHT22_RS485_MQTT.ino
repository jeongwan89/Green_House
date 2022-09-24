#define Esp01_DHT22_RS485_MQTT_02

#ifdef Esp01_DHT22_RS485_MQTT_01
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

#ifdef Esp01_DHT22_RS485_MQTT_02
#define CLIENT_NAME         "Green_House_HM100_Temp_RH_Monitor_02"
#define MQTT_PUB_PH         "Sensor/GH2/Rear/pH"
#define MQTT_PUB_HM100_TEMP "Sensor/GH2/Rear/HM_TEMP"
#define MQTT_PUB_EC         "Sensor/GH2/Rear/EC"
#define MQTT_PUB_TEMP       "Sensor/GH2/Rear/Temp"
#define MQTT_PUB_HUM        "Sensor/GH2/Rear/Hum"
#define SENSOR_STATUS       "Sensor/GH2/Rear/Stat"
#define TEMP_CAL  0
#define HUM_CAL   0
#endif

#ifdef Esp01_DHT22_RS485_MQTT_03
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

#ifdef Esp01_DHT22_RS485_MQTT_04
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

#ifdef Esp01_DHT22_RS485_MQTT_05
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

#include <EspMQTTClient.h>
#include <DHT.h>

#define SSerialTxControl 0                                  // GPIO00로 RS485방향을 제어한다.

#define CHIP485_SEL_TX digitalWrite(SSerialTxControl,HIGH)  //Transmission  - Driver Out
#define CHIP485_SEL_RX digitalWrite(SSerialTxControl,LOW)   //Receive       - Receiver Out

// rs485가 softwareSerial로 할당 되어 있어서 지웠다. Serial로 재 할당하여 coding해야 하겠다.
#define rs485 Serial

#define DHTPIN  2
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

#define REFRESH_TIME 10  //sec REFRESH_TIME 마다 온도/습도를 읽어서 MQTT에 올리기 위한 인터벌 시간

//wifi연결 환경이 달라짐에 따른 compile변수의 교정
#define TEST  //작업실 환경에서 TEST를 염두에 두고 농장 환경에서는 #ifndef TEST를 사용한다.

#ifdef TEST
EspMQTTClient client(
  "FarmMain5G",
  "wweerrtt",
  "192.168.0.24",  // MQTT Broker server ip
  "farmmain",   // Can be omitted if not needed
  "eerrtt",   // Can be omitted if not needed
  CLIENT_NAME,     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);
#elif
EspMQTTClient client(
  "FarmMain5G",
  "wweerrtt",
  "192.168.0.33",  // MQTT Broker server ip
  "farmmain",   // Can be omitted if not needed
  "eerrtt",   // Can be omitted if not needed
  CLIENT_NAME,     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);
#endif

//DHT 정의
DHT dht(DHTPIN, DHTTYPE); // Pin GPIO_2, Type DHT22

//Variables
int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value
volatile boolean state = true;

//loop()에서 시간 간격당 실행시키기 위한 변수선언
unsigned long last_refreshed_time;

unsigned char Data[16];
byte  requestData[9]={
        0x01,   //Device ID
        0x03,   //Function
        0x00,   //Address high byte
        0x68,   //Address low byte
        0x00,   //Data high byte
        0x03,   //Data low byte
        0x17,   //CRC high byte
        0x84    //CRC low byte
      }; 
      
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

    Serial.print("Demand Data to HM-100 sending:");
    for(int i=0; i<8; i++){
        sprintf(mP, "0x%02x ", requestData[i]);
        //Serial로 출력하는 대신에 MQTT로 pub해야한다.
        //Serial.print(mP);
    }
    Serial.println();

    CHIP485_SEL_RX;
}

int ReadData(void){
    int index =0;
    char mP[5];
    CHIP485_SEL_RX;

    index = rs485.available();
    if(index >0){
        //아래는 이전 code. RS485에서부터 data를 받았다는 표시이다. 필요없어서 주석처리.
        //Serial.print("Received Data:");
        for(int i=0; i<index; i++){
        Data[i] = rs485.read();
        sprintf(mP, "0x%02x ", Data[i]);
            //Serial로 출력하는 대신에 MQTT로 pub해야한다.
            //Serial.print(mP);
        }
            //Serial.print("\t"); Serial.println(index);
    }
    CHIP485_SEL_RX;
    return index;
}

float EC;
float pH;
float temp_HM100;
void Parsing(void)
{
    EC = (float)((int)Data[3]*256+(int)Data[4])/100;
    pH = (float)((int)Data[5]*256+(int)Data[6])/100;
    temp_HM100 = (float)((int)Data[7]*256+(int)Data[8])/10;

    for(int i=0; i<16; i++){
    Data[i] = 0x00;
    }
    //참고로 pH, temp, EC는 hm-100의 단위 select에 따라 달라진다.
    //Serial.print("pH = "); Serial.println(pH);
    //Serial.print("tp = "); Serial.println(temp);
    //Serial.print("EC = "); Serial.println(EC);
}


void setup(){
    pinMode(SSerialTxControl, OUTPUT);
    dht.begin();

    //Serial.begin(115200);
    rs485.begin(19200);
    CHIP485_SEL_RX;

    // Optional functionalities of EspMQTTClient
    // Enable debugging messages sent to serial output
    client.enableDebuggingMessages(); 
    
    // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. 
    //These can be overridded with enableHTTPWebUpdater("user", "password").
    client.enableHTTPWebUpdater(); 
    
    // Enable OTA (Over The Air) updates. 
    //Password defaults to MQTTPassword. Port is the default OTA port. 
    //Can be overridden with enableOTA("password", port).
    client.enableOTA(); 

    // You can activate the retain flag by setting the third parameter to true
    client.enableLastWillMessage(SENSOR_STATUS, "Fail",true);  

    // DHT센서를 refresh_time마다 읽기 위한 초기화 작업
    last_refreshed_time = millis();
}

void onConnectionEstablished()
{
    client.publish(SENSOR_STATUS, (String)"On Line", true); 
}
void loop()
{
    client.loop();
    if((millis()-last_refreshed_time) > REFRESH_TIME*1000) {
        DemandData();
        delay(500);
        ReadData();
        Parsing();
        //Read data and store it to variables hum and temp
        hum = dht.readHumidity() + HUM_CAL;
        temp= dht.readTemperature() + TEMP_CAL;
        
        //Print temp and humidity values to serial monitor
        client.publish(MQTT_PUB_HUM, (String) hum, true);
        client.publish(MQTT_PUB_TEMP, (String) temp, true);
        client.publish(MQTT_PUB_HM100_TEMP, (String) temp_HM100, true);
        client.publish(MQTT_PUB_EC, (String) EC, true);
        client.publish(MQTT_PUB_PH, (String) pH, true);
        
        last_refreshed_time = millis(); 
    }
}
