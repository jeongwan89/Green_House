#include <SoftwareSerial.h>
#include <WiFiEsp.h>
#include <WiFiEspClient.h>
//#include <WiFiEspUdp.h>
#include <PubSubClient.h>
//#include <stdlib.h>
//#include <Wire.h>
//#include <DHT.h>

int LED_PIN = 13;
int led_state = 0;
long last_send_time = 0;
char publish_msg[16];

const char* WIFI_SSID = "KT_GiGA_08C8";
const char* WIFI_PW = "cf05zc0562";
const char* MQTT_BROKER_ADDR = "172.30.1.15";
const int   MQTT_BROKER_PORT = 1883;
const char* MQTT_ID = "farmmain5g";
const char* MQTT_PW = "eerrtt";
int status = WL_IDLE_STATUS;   // the Wifi radio's status

byte mac[6];

WiFiEspClient esp8266Client;
PubSubClient mqtt_client(esp8266Client);
SoftwareSerial esp8266(10, 11); // RX, TX to ESP-01

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, led_state);

  Serial.begin(115200);
  Serial.println("Serial begin with 115200");

  esp8266.begin(9600);
  WiFi.init(&esp8266);
  // check for the presence of the shield
    if (WiFi.status() == WL_NO_SHIELD) {
        Serial.println("WiFi shield not present");
    // don't continue
    while (true);
    }
  // attempt to connect to WiFi network
    while ( status != WL_CONNECTED) {
       Serial.print("Attempting to connect to WPA SSID: ");
       Serial.println(WIFI_SSID);
       // Connect to WPA/WPA2 network
       status = WiFi.begin(WIFI_SSID, WIFI_PW);
    }
  // you're connected now, so print out the data
    Serial.println("You're connected to the network"); 
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP()); 
    Serial.print("MAC address = ");
    // Display MAC Address
        WiFi.macAddress(mac);
        Serial.print(mac[5],HEX);
        Serial.print(":");
        Serial.print(mac[4],HEX);
        Serial.print(":");
        Serial.print(mac[3],HEX);
        Serial.print(":");
        Serial.print(mac[2],HEX);
        Serial.print(":");
        Serial.print(mac[1],HEX);
        Serial.print(":");
        Serial.println(mac[0],HEX);

  // setup MQTT Client
  mqtt_client.setServer(MQTT_BROKER_ADDR, MQTT_BROKER_PORT);
  mqtt_client.setCallback(mqtt_callback);
}

void mqtt_callback(char* topic, byte* payload, unsigned int length){
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]");
  for (int i = 0; i < length ; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
  //Debug line
  //  Serial.println(length);
  //  Serial.println((char*)payload);
  //  Serial.println((char*)payload[length-3]);
  
  if(int(payload[length-1]-'0')){
    led_state = 1;
    digitalWrite(LED_PIN, led_state);
   } else {
    led_state = 0;
    digitalWrite(LED_PIN, led_state);
   }
   last_send_time = millis();
   sprintf(publish_msg, "%d", led_state);
   mqtt_client.publish("esp8266_test/led/state", publish_msg);
}

void establish_mqtt_connection(){
  if(mqtt_client.connected()) 
    return;
  while(!mqtt_client.connected()){
    Serial.println("Try to connect MQTT Broker");
    if(mqtt_client.connect("ESP8266_Client", MQTT_ID, MQTT_PW)){
      Serial.println("Connected");
      mqtt_client.subscribe("esp8266_test/led/command");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      delay(2000);
    }
  }
}
void loop() {
  establish_mqtt_connection();
  mqtt_client.loop();

  long current = millis();
  if(current - last_send_time > 5000){
    last_send_time = current;
    sprintf(publish_msg, "%d", led_state);
    mqtt_client.publish("esp8266_test/led/state", publish_msg);
  }
  delay(3000);
}
