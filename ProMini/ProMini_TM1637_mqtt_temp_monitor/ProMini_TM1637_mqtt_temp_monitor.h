#include <Arduino.h>
#include <TM1637Display.h>
#include <WiFiEspAT.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

#include <stdlib.h>
#include <stdio.h>
#define ESP8266_BAUD 57600


#define MQTTID "TempMonitorProto"
#define MQTTUSER "farmmain"
#define MQTTPASS "eerrtt"
#define WILLTOPIC "Lastwill/Monitor/Greenhouse/Tempmonitor1"
#define WILLMSG "off line"

#define CLK_1 2
#define DIO_1 3
#define CLK_2 4
#define DIO_2 5
#define CLK_3 6
#define DIO_3 7
#define CLK_4 8
#define DIO_4 9
#define CLK_5 10
#define DIO_5 11
#define CLK_6 12
#define DIO_6 13
#define CLK_7 14
#define DIO_7 15
#define CLK_8 16
#define DIO_8 17

#define SOFT_RX 18  //A4
#define SOFT_TX 19  //A5

const uint8_t SEG_DONE[] = {
	SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
	SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
	SEG_C | SEG_E | SEG_G,                           // n
	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
	};


char ssid[] = "FarmMain5G";
char pass[] = "wweerrtt";
int status = WL_IDLE_STATUS;

IPAddress server(192, 168, 0, 24);

SoftwareSerial EspSerial(SOFT_RX, SOFT_TX); //Rx, Tx
WiFiClient espClient;
PubSubClient client(espClient);

// resetFunction
void (*resetFunc)(void) = 0; //리셋함수는 어드레스가 0부터 시작하는 함수이다.

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
    int errCnt = 0;
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
            if(errCnt > 10) resetFunc();
            else errCnt++;
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
        delay(100);
        resetFunc();
    }

    Serial.print("\nAttempting to connect to WPA SSID:");
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
