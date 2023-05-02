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

#define GH1_V 8 //arduino pin 8
#define GH2_V 7 //arduino pin 7
#define GH3_V 6 //arduino pin 6
#define GH4_V 5 //arduino pin 5
#define MT_S 4  //arduino pin 4

/*************************************************************
    MQTT argument. Blynk를 썼을 때 virtual button으로 사용하였던 것을
        MQTT 서버로 바꾸고, 거기서 -m message로 받을 인수로 쓴다.
        MQTT client 프로그램을 여기에 대응하도록 짜야 한다. 
*************************************************************/
int wateringTimeSec;    //1회 관수시간 sec
int periodMin;          //4동 전체 관주하며 돌아오는 관주 주기시간 min
bool GH[5] = {false}; //온실 각동의 밸브 ex GH[1]은 1동의 valve 상태
bool MT = false;          //Motor 기동 스위치 on여부 수동일때만 고려
bool isAutoMode=false;  // isAutoMode에서는 MT상관 없이 자동모드 실행
bool prevAutoMode=false;     // prevAutoMode는 수동/자동 버튼의 이전 상태를 기억함.
unsigned long wateringStart;
unsigned long lastWateringTime;

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