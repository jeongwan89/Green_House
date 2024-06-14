#include "ProMini_Esp01_DHT_MQTT.h"

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(115200);
    EspSerial.begin(ESP8266_BAUD);
    WiFi.init(&EspSerial);
    
    Serial.println("We are in setup()");
    wifiConnect();

    client.setServer(server, 1883);
    client.setCallback(callback);
    
    
}

void loop()
{
// 파트1 ===========================================================================
// client.loop()
    if(!client.connected()) {
        reconnect();
    }
    client.loop();
}