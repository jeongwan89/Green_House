#include <WiFiEspAT.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

#include <stdlib.h>

#define ESP8266_BAUD 57600

char ssid[] = "FarmMain5G";
char pass[] = "wweerrtt";
int status = WL_IDLE_STATUS;

IPAddress server(192, 168, 0, 24);

SoftwareSerial EspSerial(8, 7); //Rx, Tx
WiFiClient espClient;
PubSubClient client(espClient);

#define MQTTID "levelMonitor"
#define MQTTUSER "farmmain"
#define MQTTPASS "eerrtt"
#define WILLTOPIC "Lastwill/FM/fertank/status"
#define WILLMSG "off line"

#define TRIG 2
#define ECHO 3

void callback(char* topic, byte* payload, unsigned int length) {
    char str[256];
    int conv;
    Serial.print("Message arrived []");
    Serial.print(topic);
    Serial.print("]");
    for(int i = 0; i < length; i++){
        Serial.print((char) payload[i]);
        str[i] = (char) payload[i];
        str[i+1] = NULL;
    }
    // 이하 특별한 topic message에 대한 처리 루틴이 있어야한다.
}

void reconnect(void) {
    while(!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if(client.connect(MQTTID,MQTTUSER,MQTTPASS,WILLTOPIC, 0, 1, WILLMSG)) {
            Serial.print("connected");
            client.publish(WILLTOPIC, "on line");
            // client.subscribe("...");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.print("\t try again in 5 seconds");
            delay(5000);
        }
    }
}

void wifiConnect(void) 
{
/* setup() 안에서 작동한다. 이전에 반드시 정의 되어야할 것이 있는데, 
    ESP01이 SoftwareSerial로 정의되어 있어야 하고(EspSerial),
    WiFi 쉴드 변수와 ESP01모뎀의 객체의 레퍼런스를 연결해 주어야 한다.
*/
    EspSerial.begin(ESP8266_BAUD);
    WiFi.init(&EspSerial);

    if(WiFi.status() == WL_NO_SHIELD) {
        Serial.println("WiFi shield not present");
        while(true);
    }

    Serial.print("Attempting to connect to WPA SSID:");
    Serial.println(ssid);
    
    WiFi.begin(ssid, pass);

    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("Your're connected to the networtk");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}