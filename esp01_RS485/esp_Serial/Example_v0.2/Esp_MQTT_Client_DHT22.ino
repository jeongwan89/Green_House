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

#include <EspMQTTClient.h>
#include <DHT.h>

// RelayPin 정의
#define RELAY1  0
#define RELAY2  1 //LED & TX와 같은 핀

#define DHTPIN  2
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 3 (on the right) of the sensor to GROUND (if your sensor has 3 pins)
// Connect pin 4 (on the right) of the sensor to GROUND and leave the pin 3 EMPTY (if your sensor has 4 pins)
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
#define REFRESH_TIME 7  //sec REFRESH_TIME 마다 온도/습도를 읽어서 MQTT에 올리기 위한 인터벌 시간

EspMQTTClient client(
  "FarmMain5G",
  "wweerrtt",
  "192.168.0.26",  // MQTT Broker server ip
  "farmmain",   // Can be omitted if not needed
  "eerrtt",   // Can be omitted if not needed
  "HeaterControl_H1F",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);
\
/* ESP-01의 모든 핀을 사용하여야 한다.
  DHTPin  GPIO2
  Relay1  GPIO0
  Relay2  GPIO1
*/
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
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  dht.begin();
  // Serial.begin(115200);

  // Optional functionalities of EspMQTTClient
  // client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overridded with enableHTTPWebUpdater("user", "password").
  client.enableOTA(); // Enable OTA (Over The Air) updates. Password defaults to MQTTPassword. Port is the default OTA port. Can be overridden with enableOTA("password", port).
  client.enableLastWillMessage("home/lastwill", "I am going offline");  // You can activate the retain flag by setting the third parameter to true

  // DHT센서를 refresh_time마다 읽기 위한 초기화 작업
  last_refreshed_time = millis();
}

// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished()
{
  /* Serial 통신을 포기하였기 때문에 아래 code는 수정이 많이 필요하여 주석차리하고 
    주석 아래에다 새로운 코드를 작성한다.
  // Subscribe to "monitor/temp" and display received message to Serial
  client.subscribe("monitor/temp", [](const String & payload) {
    Serial.println(payload);
  });
  // Subscribe to "monitor/#" and display received message to Serial
  client.subscribe("monitor/#", [](const String & topic, const String & payload) {
    Serial.println("(From wildcard) topic: " + topic + ", payload: " + payload);
  });
  // Publish a message to "monitor/hum"
  client.subscribe("monitor/hum", [](const String & payload) {
    Serial.println(payload);
  }); // You can activate the retain flag by setting the third parameter to true
  */
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
/*
  // Execute delayed instructions
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
    hum = dht.readHumidity();
    temp= dht.readTemperature();
    
    client.publish("Sensor/HM/Center/Temp", (String) temp);
    client.publish("Sensor/HM/Center/Hum", (String) hum);

    //아래 코드는 Serial통신을 포기하고 TX를 GPIO 0으로 쓰기 위함
    /* //Print temp and humidity values to serial monitor
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.println(" %");
    client.publish("monitor/hum", (String) hum);

    Serial.print("Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");
    client.publish("monitor/temp", (String) temp);
    */
    client.publish("Actuator/HM/Center/Heater", (String) state);
    client.publish("Actuator/HM/Center/Fuel", (String) state);
    state = !state;

    last_refreshed_time = millis(); 
  }
}
