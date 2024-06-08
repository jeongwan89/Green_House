#include "Blink.h"
#include "mylib.h"
float dhtTemp;
float dhtHum;

RaiseTimeEventInLoop readDHT;
RaiseTimeEventInLoop mqttPub;
// the setup function runs once when you press reset or power the board
void setup()
{

    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(115200);
    Serial1.begin(ESP8266_BAUD);
    WiFi.init(&Serial1, ESP01_RESET);
    Serial.print("Now entering wifiConnect()\n");
    wifiConnect();
    client.setServer(server, 1883);
    client.setCallback(callback);

    // TM1637 setup
    dht.begin();
}

// the loop function runs over and over again forever
void loop()
{
    blink(1, 200);
    // 파트1 ===================================================================================
    // mqtt 관련 함수를 먼저 refresh하기 위해서이다.
    if (!client.connected())
    {
        reconnect();
    }
    client.loop();

    // 파트2====================================================================================
    readDHT.eachEveryTimeIn(5000, f_readDHT);
    mqttPub.eachEveryTimeIn(5000, pubmqtt);
}
