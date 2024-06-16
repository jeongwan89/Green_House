#include "ProMini_TM1637_mqtt_temp_monitor.h"

void setup()
{
    initialDisplay();

    Serial.begin(115200);
    EspSerial.begin(ESP8266_BAUD);
    WiFi.init(&EspSerial);

    wifiConnect();

    client.setServer(server, 1883);
    client.setCallback(callback);
}

void loop()
{
    // 파트1 =======================================================================
    if(!client.connected()) {
        reconnect();
    }
    client.loop();
    // 파트2 =======================================================================
    
}