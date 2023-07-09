/*
  SimpleMQTTClient.ino
  The purpose of this exemple is to illustrate a simple handling of MQTT and Wifi connection.
  Once it connects successfully to a Wifi network and a MQTT broker, it subscribe to a topic and send a message to it.
  It will also send a message delayed 5 seconds later.

  ESP07 -> ESO07_Example.ino -> ESP07_Read_HM100.ino -> ESP07_Read_HM100_MQTT.ino
  이 코드의 목적은 HM100에서 EC pH Temp_drain를 RS485에서 읽고
  그 온도 값을 MQTT서버에 올리는 일이다.
*/
/*
  아래에서 MQTT Topic의 구조를 유지한다.
  Sensor/Topos1/Topos2/Name/Stat
  Actuator/Topos1/Topos2/Name/Act
 */

/*
  #define구문을 정의한다.
  동마다 위치마다 MQTT의 publish가 다르기 때문에 여기에 한번에 걸쳐서 MQTT의 구조를 유지한다.
  장점으로는 파일을 여러개 만들지 않아도 된다.
  단점으로는 컴파일할 때 define의 정의를 명백히 고쳐서 컴파일 해야한다.
  define의 정의를 고쳐서 컴파일하는 일을 컴파일 인자를 고쳐 컴파일한다고 하겠다.
  #define구문의 정의는 Esp07_HM100_MQTT_01 ~ xx (100개)로 하고 각 define의 인자를 정의하는 것으로 한다.
*/
#define Esp07_HM100_MQTT_02

#ifdef Esp07_HM100_MQTT_01
#define CLIENT_NAME         "Green_House_HM100_Monitor_01"
#define MQTT_PUB_TEMP_DRAIN "Sensor/GH1/Rear/Temp_Drain"        //GH1 = Green House (온실) 1동
#define MQTT_PUB_EC         "Sensor/GH1/Rear/EC"
#define MQTT_PUB_PH         "Sensor/GH1/Rear/PH"
#define SENSOR_STATUS       "Sensor/GH1/Rear/Stat"
#define TEMP_DRAIN_CAL      0
#define EC_CAL              0
#endif

#ifdef Esp07_HM100_MQTT_02
#define CLIENT_NAME         "Green_House_HM100_Monitor_02"
#define MQTT_PUB_TEMP_DRAIN "Sensor/GH2/Rear/Temp_Drain"
#define MQTT_PUB_EC         "Sensor/GH2/Rear/EC"
#define MQTT_PUB_PH         "Sensor/GH2/Rear/PH"
#define SENSOR_STATUS       "Sensor/GH2/Rear/Stat"
#define TEMP_DRAIN_CAL      0
#define EC_CAL              0
#endif

#ifdef Esp07_HM100_MQTT_03
#define CLIENT_NAME         "Green_House_HM100_Monitor_03"
#define MQTT_PUB_TEMP_DRAIN "Sensor/GH3/Rear/Temp_Drain"
#define MQTT_PUB_EC         "Sensor/GH3/Rear/EC"
#define MQTT_PUB_PH         "Sensor/GH3/Rear/PH"
#define SENSOR_STATUS       "Sensor/GH3/Rear/Stat"
#define TEMP_DRAIN_CAL      0
#define EC_CAL              0
#endif

#ifdef Esp07_HM100_MQTT_04
#define CLIENT_NAME         "Green_House_HM100_Monitor_04"
#define MQTT_PUB_TEMP_DRAIN "Sensor/GH4/Rear/Temp_Drain"
#define MQTT_PUB_EC         "Sensor/GH4/Rear/EC"
#define MQTT_PUB_PH         "Sensor/GH4/Rear/PH"
#define SENSOR_STATUS       "Sensor/GH4/Rear/Stat"
#define TEMP_DRAIN_CAL      0
#define EC_CAL              0
#endif

#ifdef Esp07_HM100_MQTT_05
#define CLIENT_NAME         "NR_House_HM100_Monitor_05"
#define MQTT_PUB_TEMP_DRAIN "Sensor/NR1/Rear/Temp_Drain"          //NR1 = Nursery Green House (육묘장) 1동
#define MQTT_PUB_EC         "Sensor/NR1/Rear/EC"
#define MQTT_PUB_PH         "Sensor/NR1/Rear/PH"
#define SENSOR_STATUS       "Sensor/NR1/Rear/Stat"
#define TEMP_DRAIN_CAL      0
#define EC_CAL              0
#endif

#include <EspMQTTClient.h>
#include <SoftwareSerial.h>

#define SSerialTxControl 16

#define CHIP485_SEL_TX digitalWrite(SSerialTxControl,HIGH)  //Transmission
#define CHIP485_SEL_RX digitalWrite(SSerialTxControl,LOW)   //Receive

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

//Variables
int chk;
float EC;
float pH;
float temp_drain;

volatile boolean state = true;

//loop()에서 시간 간격당 실행시키기 위한 변수선언
unsigned long last_refreshed_time;

SoftwareSerial rs485(12,13); //(RX TX)
unsigned char Data[16];
byte  requestData[9]={
        0x01,   //Device ID
        0x03,   //Function
        0x00,   //Address high byte
        0x68,   //Address low byte
        0x03,   //Data low byte
        0x84,   //CRC low byte
        0x17    //CRC high byte
      }; 

// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished()
{
    client.publish(SENSOR_STATUS, (String)"On Line", true); 
    // You can activate the retain flag by setting the third parameter to true
    //아래 코드는 그 이전 프로젝트에서 받은 코드이다.
    //Relay 작동을 하지 않는 프로그램이어서, MQTT-> Actuator 서브스크라이브를 하지 않게 되었다.
    //그래서 남은 코드의 흔적을 아래에 둔다.
    /*
    client.subscribe("Actuator/HM/Rear/Heater",[](const String & payload){
        if(payload == "1"){
        digitalWrite(RELAY1, HIGH); 
        } else {
        digitalWrite(RELAY1, LOW);
        }
    });
    client.subscribe("Actuator/HM/Rear/Fuel",[](const String & payload){
        if(payload == "1"){
        digitalWrite(RELAY2, HIGH);
        } else {
        digitalWrite(RELAY2, LOW);
        }
    });
    */
/*
    // Execute delayed instructions
    // 아래 코드는 예제에서 소개된 코드로 지정된 시간뒤에 어떤 동작을 하는 코드이다.
    client.executeDelayed(5 * 1000, []() {
        client.publish("mytopic/wildcardtest/test123", "This is a message sent 5 seconds later");
    });
*/
}

    
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
        Serial.print(mP);
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
        Serial.print("Received Data:");
        for(int i=0; i<index; i++){
        Data[i] = rs485.read();
        sprintf(mP, "0x%02x ", Data[i]);
        Serial.print(mP);
        }
        Serial.print("\t"); Serial.println(index);
    }
    CHIP485_SEL_RX;
    return index;
}

void Parsing(void){

    EC = (float)((int)Data[3]*256+(int)Data[4])/100;
    pH = (float)((int)Data[5]*256+(int)Data[6])/100;
    temp_drain = (float)((int)Data[7]*256+(int)Data[8])/10;

    for(int i=0; i<16; i++){
        Data[i] = 0x00;
    }
    //참고로 pH, temp, EC는 hm-100의 단위 select에 따라 달라진다.
    Serial.print("pH = "); Serial.print(pH); Serial.print('\t');
    Serial.print("tp = "); Serial.print(temp_drain); Serial.print('\t');
    Serial.print("EC = "); Serial.println(EC);
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(SSerialTxControl, OUTPUT);

    Serial.begin(115200);
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

    //refresh_time마다 읽기 위한 초기화 작업
    last_refreshed_time = millis();
}

void loop()
{
    client.loop();
    if((millis()-last_refreshed_time) > REFRESH_TIME*1000) {
        DemandData();
        delay(100);
        if(ReadData()!=15) return;
        Parsing();
        delay(100);        

        //Read data and store it to variables hum and temp
        // hum = dht.readHumidity() + HUM_CAL;
        
        // //Print temp_media and humidity values to serial monitor
        // Serial.print("Humidity: ");
        // Serial.print(hum);
        // Serial.println(" %");
        // client.publish(MQTT_PUB_EC, (String) EC, true);

        client.publish(MQTT_PUB_PH, (String) pH, true);
        client.publish(MQTT_PUB_TEMP_DRAIN, (String) temp_drain, true);
        client.publish(MQTT_PUB_EC, (String) EC, true);

        last_refreshed_time = millis(); 
    }
}

