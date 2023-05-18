#include <WiFiEspAT.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

#include <stdlib.h>
#include <stdio.h>
#define ESP8266_BAUD 57600

char ssid[] = "FarmMain5G";
char pass[] = "wweerrtt";
int status = WL_IDLE_STATUS;

IPAddress server(192, 168, 0, 24);

SoftwareSerial EspSerial(8, 7); //Rx, Tx
WiFiClient espClient;   //wifiespAT에서 정의됨
PubSubClient client(espClient);

#define MQTTID "levelMonitor"
#define MQTTUSER "farmmain"
#define MQTTPASS "eerrtt"
#define WILLTOPIC "Lastwill/FM/fertank/status"
#define WILLMSG "off line"

#define TRIG 2
#define ECHO 3

/*
    이 콜백에서는 따로 payload뿐만 아니라 str[256]에서 카피를 받아쓴다.
    payload마지막에 NULL이 있는지 없는지는 모르겠지만,
    str에는 마지막에 NULL이 있어서 string으로 쓸수 있다.
*/
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
            client.publish(WILLTOPIC, "on line", 1);
            // client.subscribe("...");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.print("\t try again in 5 seconds\n");
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

int sumDist = 0;
int avrDistance(unsigned int distance)
{
    static unsigned int arrDistance[128] = {0,};
    static int index = 0;

    if(index == 128) index=0;
    arrDistance[index] = distance;
    sumDist = sumDist + arrDistance[index] - arrDistance[(index == (128-1) ? 0 : index+1)];
    index ++;
    //Serial.print("index :"); Serial.println(index);
    //Serial.print("arrDistance["); Serial.print(index); Serial.print("] = "); Serial.println(arrDistance[(index == 0 ? 127 : index-1)]);
    //Serial.println(sumDist);
    return (int) (sumDist/127);
}

// 사각 탱크의 남은 용적을 구한다. 일단 100세티로 계산하고 거리를 받아서 %로 환산해서 리턴한다.
int volumePerc(int dist)
{
    return ((-1.25)*dist + 112.5);
}