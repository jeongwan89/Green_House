/*
  SimpleMQTTClient.ino
  The purpose of this exemple is to illustrate a simple handling of MQTT and Wifi connection.
  Once it connects successfully to a Wifi network and a MQTT broker, it subscribe to a topic and send a message to it.
  It will also send a message delayed 5 seconds later.

  Esp_MQTT_Client_DHT22.ino
  (simpleMQTTClient.ino에서 코드를 승계하였음)
  이 코드의 목적은 DHT22에서 온도를 읽고
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
  #define구문의 정의는 Esp01_DHT22_MQTT_01 ~ xx (100개)로 하고 각 define의 인자를 정의하는 것으로 한다.
*/
#define Esp01_DHT22_MQTT_03

#ifdef Esp01_DHT22_MQTT_01
#define CLIENT_NAME   "Green_House_Temp_RH_Monitor_01"
#define MQTT_PUB_TEMP "Sensor/GH1/Center/Temp"        //GH1 = Green House (온실) 1동
#define MQTT_PUB_HUM  "Sensor/GH1/Center/Hum"
#define SENSOR_STATUS "Sensor/GH1/Center/Stat"
#define TEMP_CAL  0
#define HUM_CAL   2.3
#endif

#ifdef Esp01_DHT22_MQTT_02
#define CLIENT_NAME   "Green_House_Temp_RH_Monitor_02"
#define MQTT_PUB_TEMP "Sensor/GH2/Center/Temp"
#define MQTT_PUB_HUM  "Sensor/GH2/Center/Hum"
#define SENSOR_STATUS "Sensor/GH2/Center/Stat"
#define TEMP_CAL  0.6
#define HUM_CAL   0
#endif

#ifdef Esp01_DHT22_MQTT_03`
#define CLIENT_NAME   "Green_House_Temp_RH_Monitor_03"
#define MQTT_PUB_TEMP "Sensor/GH3/Center/Temp"
#define MQTT_PUB_HUM  "Sensor/GH3/`Center`/Hum"
#define SENSOR_STATUS "`Sensor/GH3/Center/Stat"
#define TEMP_CAL  -1.3
#define HUM_CAL   4.4
#endif

#ifdef Esp01_DHT22_MQTT_04
#define CLIENT_NAME   "Green_House_Temp_RH_Monitor_04"
#define MQTT_PUB_TEMP "Sensor/GH4/Center/Temp"
#define MQTT_PUB_HUM  "Sensor/GH4/Center/Hum"
#define SENSOR_STATUS "Sensor/GH4/Center/Stat"
#define TEMP_CAL  0.1
#define HUM_CAL   -1
#endif

#ifdef Esp01_DHT22_MQTT_05
#define CLIENT_NAME   "NR_House_Temp_RH_Monitor_01"
#define MQTT_PUB_TEMP "Sensor/NR1/Center/Temp"          //NR1 = Nursery Green House (육묘장) 1동
#define MQTT_PUB_HUM  "Sensor/NR1/Center/Hum"
#define SENSOR_STATUS "Sensor/NR1/Center/Stat"
#define TEMP_CAL  0
#define HUM_CAL   0
#endif

#include <EspMQTTClient.h>
#include <DHT.h>

/* ESP-01의 모든 핀을 사용하여야 한다. -> 정책취소
  DHTPin  GPIO2
  // Relay1  GPIO0
  // Relay2  GPIO1
*/
/*위의 정책을 포기하고 DHTPin=GPIO2만 쓰는 것으로 회귀한다.
그 이유로는 전원회로가 복잡하고 오로지 DHT22만으로도 충분하기 때문이다.
*/
// RelayPin 정의 -> 사용하지 않음
// #define RELAY1  0
// #define RELAY2  1 //LED & TX와 같은 핀

#define DHTPIN  2
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 3 (on the right) of the sensor to GROUND (if your sensor has 3 pins)
// Connect pin 4 (on the right) of the sensor to GROUND and leave the pin 3 EMPTY (if your sensor has 4 pins)
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor (PULL-UP Resistor)
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

void setup()
{
  /* 이 프로젝트는 릴레이를 쓰지 않겠다.
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  */

  dht.begin();
  Serial.begin(115200);

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
  client.subscribe("Actuator/HM/Center/Heater",[](const String & payload){
    if(payload == "1"){
      digitalWrite(RELAY1, HIGH); 
    } else {
      digitalWrite(RELAY1, LOW);
    }
  });
  client.subscribe("Actuator/HM/Center/Fuel",[](const String & payload){
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

void loop()
{
  client.loop();
  if((millis()-last_refreshed_time) > REFRESH_TIME*1000) {
    //Read data and store it to variables hum and temp
    hum = dht.readHumidity() + HUM_CAL;
    temp= dht.readTemperature() + TEMP_CAL;
    
    //Print temp and humidity values to serial monitor
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.println(" %");
    client.publish(MQTT_PUB_HUM, (String) hum, true);

    Serial.print("Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");
    client.publish(MQTT_PUB_TEMP, (String) temp, true);
    
    last_refreshed_time = millis(); 
  }
}
