#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WiFiEspAT.h> //ESP01이 modem으로 사용된다, serial을 통해서.
#include <PubSubClient.h>
#include "Arduino.h"
#include "MQTT_def.h"
char ssid[] = "FarmMain5G";
char pass[] = "wweerrtt";
int status = WL_IDLE_STATUS;

IPAddress server(192, 168, 0, 24);

// WiFiClient와 PubSubClient는 같은 쌍이다.
WiFiClient espClient; // WiFiEspAT에서 정의됨
PubSubClient client(espClient);

#define ESP8266_BAUD 115200

// pin 번호 정의
// Serial1 RX=0; TX=1  ---> ESP01연결
#define SERIAL1_TX 0
#define SERIAL1_RX 1
#define ESP01_RESET 2
#define DHTPIN 3
#define TM1637CLK 4
#define TM1637DIO 5

void blink(int, int);
void resetFunct(void);
void wifiConnect(void);
void callback(char *, byte *, unsigned int);
void reconnect(void);
void f_readDHT(void);
//===============================================================================================
// 함수 정의 definition
//===============================================================================================

// resetFunction
void resetFunc(void)
// 리셋함수는 어드레스가 0부터 시작하는 함수이다.
{
    watchdog_enable(1, 1);
    while (1)
        ;
}

/* setup() 안에서 작동한다. 이전에 반드시 정의 되어야할 것이 있는데,
    ESP01이 SoftwareSerial로 정의되어 있어야 하고(EspSerial),
    WiFi 쉴드 변수와 ESP01모뎀의 객체의 레퍼런스를 연결해 주어야 한다.
*/
void wifiConnect(void)
{
    if (WiFi.status() == WL_NO_SHIELD)
    {
        Serial.println("WiFi shield not present");
        delay(100);
        resetFunc();
    }

    Serial.print("Attempting to connect to WPA SSID:");
    Serial.println(ssid);
    int cnt = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        WiFi.begin(ssid, pass);
        if (cnt > 50)
            resetFunc();
        cnt++;
        Serial.print(".");
    }
    blink(1, 100);
    Serial.println("\nYour're connected to the networtk");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

/*
    이 콜백에서는 따로 payload뿐만 아니라 str[256]에서 카피를 받아쓴다.
    payload마지막에 NULL이 있는지 없는지는 모르겠지만,
    str에는 마지막에 NULL이 있어서 string으로 쓸수 있다.
*/
void callback(char *topic, byte *payload, unsigned int length)
{
    char str[256];
    int conv;
    Serial.print("Message arrived []");
    Serial.print(topic);
    Serial.print("]");
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
        str[i] = (char)payload[i];
        str[i + 1] = '\0';
    }
    // 이하 특별한 topic message에 대한 처리 루틴이 있어야한다.
}

void reconnect(void)
{
    int errCnt = 0;
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        if (client.connect(CLIENT_NAME, MQTTUSER, MQTTPASS, WILLTOPIC, 0, 1, WILLMSG))
        {
            Serial.print("\n");
            client.publish(WILLTOPIC, "onLine", 1);
            Serial.println("MQTT connected");
            Serial.println("WillTopic is published as onLine");
            blink(3, 100);
            // client.subscribe("...");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.print("\t try again in 5 seconds\n");
            delay(5000);
            if (errCnt > 10)
                resetFunc();
            else
                errCnt++;
            blink(1, 500);
        }
    }
}

//============================================================================================
//     TM1637.h
//============================================================================================
#include <TM1637Display.h>

const uint8_t SEG_DONE[] = {
    SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,         // d
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F, // O
    SEG_C | SEG_E | SEG_G,                         // n
    SEG_A | SEG_D | SEG_E | SEG_F | SEG_G          // E
};

TM1637Display display(TM1637CLK, TM1637DIO);

//============================================================================================
//     DHT.h
//============================================================================================
#include <DHT.h>
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

//--------------------------------------------------------------------------------------------

extern float dhtTemp;
extern float dhtHum;
void f_readDHT(void)
{
    dhtTemp = dht.readTemperature();
    dhtHum = dht.readHumidity();
    Serial.printf("DHT Temp: %2.1f °C\t", dhtTemp);
    Serial.printf("DHT Hum : %2.1f %%\n", dhtHum);
}

//============================================================================================
//  Publish
//============================================================================================
void pub(const char *tpc, const char *msg)
{
    client.publish(tpc, msg, 1);
}

void pub(const char *tpc, int msg)
{
    char buffer[128];
    itoa(msg, buffer, 10);
    client.publish(tpc, buffer, 1);
}

/**
 * @brief   설치되어 있는 LED를 점등한다.
 *
 * @param   {int}   times 깜빡이는 회수
 * @param   {int}   duration_millis 점등되는 시간 millisecond
 */
void blink(int times, int duration_millis)
{
    for (int i = 0; i < times; i++)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(duration_millis);
        digitalWrite(LED_BUILTIN, LOW);
        delay(duration_millis);
    }
}

/**
 * @brief /Sensor/GH1/Center/AirTemp과 /AirHum에 쓰기
*/
void pubmqtt(void)
{
    char temp[10];
    char hum[10];
    sprintf(temp, "%.1f", dhtTemp);
    sprintf(hum, "%.1f", dhtHum);
    client.publish(TPC_AIRTEMP, temp, 1);
    client.publish(TPC_AIRHUM, hum, 1);
}