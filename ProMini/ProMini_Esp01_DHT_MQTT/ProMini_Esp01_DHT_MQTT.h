#ifndef PROMINI_ESP01_DHT_MQTT
#define PROMINI_ESP01_DHT_MQTT
#include <stdio.h>
#include <stdlib.h>
#include <Arduino.h>
#include <WiFiEspAT.h>
#include <SoftwareSerial.h>
#include <PubSubClient.h>

#define ESP8266_BAUD 57600

#define MQTTID "promini_test_1"
#define MQTTUSER "farmmain"
#define MQTTPASS "eerrtt"
#define WILLTOPIC "Lastwill/Test/Proto"
#define WILLMSG "off line"

char ssid[] = "FarmMain5G";
char pass[] = "wweerrtt";
int status = WL_IDLE_STATUS;

IPAddress server(192, 168, 0, 24);

SoftwareSerial EspSerial(18, 19); //RX(A4), TX(A5)
WiFiClient espClient;
PubSubClient client(espClient);

// reset함수가 이름이 바뀌었다. resetProMini <- resetFunc
void (*resetProMini) (void) = 0; // 리셋함수는 어드레스가 0부터 시작하는 함수이다.

/**
 * 이 콜백에서는 따로 payload뿐만 아니라 str[256]에서 카피를 받아쓴다.
 * payload마지막에 NULL이 있는지 없는지 모르겠지만,
 * str에는 마지막에 NULL이 있어서 string으로 쓸 수 있다. 
 */
void callback(char* topic, byte* payload, unsigned int length)
{
    char str[256];
    int conv;
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("]");
    for(int i = 0; i < length; i++){
        Serial.print((char) payload[i]);
        str[i] = (char) payload[i];
        str[i+1] = '\0';
    }

    // 이하 이 topic message 처리 루틴이 있어야 한다.
}

void reconnect(void)
{
    int errCnt = 0;
    while(!client.connected()) {
        Serial.print("Attempting MQTT conncection...");
        if(client.connect(MQTTID, MQTTUSER, MQTTPASS, WILLTOPIC, 0, 1, WILLMSG)) {
            Serial.print("connected!");
            client.publish(WILLTOPIC, "on line", 1);
            // client.subscribe("...")
        } else {
            Serial.print("Failed, rc=");
            Serial.print(client.state());
            Serial.print("\t try again in 5 second\n");
            delay(5000);
            if(errCnt > 10) resetProMini();
            else errCnt++;
        }
    }
}

void wifiConnect(void)
{
    /**
     * Setup() 안에서 작동한다. 이전에 반드시 정의되어야 할 것이 있는데,
     * ESP01이 SoftwareSerial로 정의되어 있어야 하고(EspSerial),
     * WiFi 쉴드 변수와 ESP01 모뎀 객체의 레퍼런스를 연겨해 주어야 한다.
     * 예)     WiFi.init(&EspSerial);
     */
    if(WiFi.status() == WL_NO_SHIELD) {
        Serial.println("WiFi shield is not present");
        delay(100); // Serial에 쓰기 위한 딜레이이다. 이게 없으면 쓰다가 그냥 꺼진다.
        resetProMini();
    }

    Serial.print("Attempting to connect to WPA SSID:");
    Serial.println(ssid);

    WiFi.begin(ssid, pass);

    int errCnt = 0;
    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        if (errCnt > 20) resetProMini();
        else errCnt++;
    }

    Serial.print("You're conncected to the network!\n");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}
#endif