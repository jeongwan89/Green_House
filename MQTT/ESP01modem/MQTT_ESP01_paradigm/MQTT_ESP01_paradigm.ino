/*************************************************************
    IMPORT :
        WiFiESP.h
        Pubsubclient.h
        SoftwareSerial.h
**************************************************************/
#include <WiFiEsp.h>            //https://github.com/bportaluri/WiFiEsp
#include <PubSubClient.h>       //https://github.com/knolleary/pubsubclient
#include <SoftwareSerial.h>

/*************************************************************
    My WiFi credentials (WPA) 이것은 WiFiEsp.h에 extern으로 정의되어 있음
**************************************************************/
#define ESP8266_BAUD 9600
SoftwareSerial EspSerial(2,3);  //RX, TX
char ssid[] = "FarmMain5G";
char pass[] = "wweerrtt";
int status = WL_IDLE_STATUS;     // the Wifi radio's status
IPAddress server(192, 168, 0, 24);  //mqtt에서 필요한 서버 IP
WiFiEspClient espClient;            // ─┬─ 이것과 아래 2개는 쌍으로 작동한다.
PubSubClient client(espClient);     //  └─ PubSubClient.h에서 필요한 것임

// MQTT authentification
#define MQTTID "arduinoControl"
#define MQTTUSER "farmmain"
#define MQTTPASS "eerrtt"

/*************************************************************
    필요한 함수를 정의하는 곳
**************************************************************/
//print any message received for subscribed topic
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup()
{
    // initialize serial for debugging
    Serial.begin(115200);
    // initialize serial for ESP module
    EspSerial.begin(ESP8266_BAUD);
    // initialize ESP module
    WiFi.init(&EspSerial);

    // check for the presence of the shield
    if (WiFi.status() == WL_NO_SHIELD) {
        Serial.println("WiFi shield not present");
        // don't continue
        // (∨)이 상황에서 WiFi 쉴드가 없다는 사실을 mqtt에 알려야 하지 않을까?
        //  └─ 쉴드가 없는데 어떻게 mqtt에 알릴 수 있는가. 없다!
        while (true);
    }

    // attempt to connect to WiFi network
    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network
        status = WiFi.begin(ssid, pass);
    }

    Serial.println("You're connected to the network");
    // (∨)연결되었으니 mqtt에 연결되었음을 신고하고, 유언 메세지도 등록한다.
    //  └─ MQTT에 연결이 안되었으니 이런 시도는 무산될 것임. 작동한할 것임으로 시도하지 않음
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    //connect to MQTT server
    client.setServer(server, 1883);
    client.setCallback(callback);
}
/*************************************************************
    loop()에서 필요한 함수를 정의하는 곳
**************************************************************/
void reconnect() 
/* 
    일종의 재귀적 용법의 함수 같아 보인다.
    client.connect()가 한번도 실행되지 않았는데 loop()안에서 reconnect()가 실행되고
    나서야 그 안의 client.connect()가 실행된다. 
    장접 : 연결이 끊기면 언제나 이 함수에 들어와서 connect() 실행한다.
    중요점: publish(const char * topic, const char * payload)
            subscribe(const char * topic)
*/
{
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect, just a name to identify the client
        if (client.connect(MQTTID, MQTTUSER, MQTTPASS)) {
            Serial.println("connected");
            // Once connected, publish an announcement...
            client.publish("command","hello world");
            // ... and resubscribe
            client.subscribe("presence");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println("try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}
 
void loop()
/*
    두 구역으로 나눈다.
    1구역 : loop()중에 연결이 확실한지 처리한다. 
            reconnect()에 MQTT topic을 늘 재정의한다.
    2구역 : pocket이 있다. 이 포켓은 programmed watering을 실행하는 곳이다.
            이 포켓에 들어가기 위한 전역변수들을 정의해야 한다.
*/
{
    //1구역
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
        while ( status != WL_CONNECTED) {
            Serial.print("Attempting to connect to WPA SSID: ");
            Serial.println(ssid);
            // Connect to WPA/WPA2 network
            status = WiFi.begin(ssid, pass);
            delay(500);
        }
        Serial.println("Connected to AP");
        IPAddress ip = WiFi.localIP();
  
        Serial.print("IP Address: ");
        Serial.println(ip);
    }
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    //2구역
    
}