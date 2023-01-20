#include <SoftwareSerial.h>
#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspUdp.h>
#include <PubSubClient.h> //https://pubsubclient.knolleary.net/api
#include <stdlib.h>
#include <Wire.h>
#include <DHT.h>

#define DHTPIN 2   
#define DHTTYPE    DHT11

char temperatureString[5];
char humidityString[5];

IPAddress server(172, 30, 1, 28);  //your mqtt server adress

char ssid[] = "KT_GiGA_08C8";           // your network SSID (name)
char pass[] = "cf05zc0562";           // your network password
int status = WL_IDLE_STATUS;   // the Wifi radio's status 
DHT dht(DHTPIN, DHTTYPE);

WiFiEspClient esp8266Client;
PubSubClient client(esp8266Client);
//SoftwareSerial esp8266(10, 11); // RX, TX to ESP-01

void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);  //software serial to ESP8266
  WiFi.init(&Serial1); //ESP8266 wifi
  client.setKeepAlive(40);
  dht.begin();     //DHT11 sensor
  // check for the presence of the shield
    if (WiFi.status() == WL_NO_SHIELD) {
        Serial.println("WiFi shield not present");
    // don't continue
    while (true);
    }
  // attempt to connect to WiFi network
    while ( status != WL_CONNECTED) {
       Serial.print("Attempting to connect to WPA SSID: ");
       Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
    }
  // you're connected now, so print out the data
    Serial.println("You're connected to the network");
  //connect to MQTT server
    client.setServer(server,1883);
    //client.setCallback(callback);
    /*--내코드 username과 password때문에 connect함수 호출 --*. 
     * 
    내코드 삽입 후 다시 삭제. loop()에 connect 멤버함수가 있어서 거기서 처리하는 것으로 함*/
    /* client.connect("FromArduinoLeonardo1", "farmmain5g", "eerrtt"); */
}

void loop(){
  int temperature = dht.readTemperature();
  int humidity = dht.readHumidity();
  
  //내 코드 첨가
  client.loop();
  dtostrf(temperature,3,0,temperatureString);
  dtostrf(humidity,3,0,humidityString);
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("C, Humidity: ");    
  Serial.print(humidity);
  Serial.println("%");
 
  // mqtt out
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    client.connect("FromArduinoLeonardo1", "farmmain5g", "eerrtt");
    Serial.println("connected");
    delay (1000);
  } 
  client.publish("temperature",temperatureString);
  delay (1000);
  client.publish("humidity",humidityString);
  delay (1000);
}
