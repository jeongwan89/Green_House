#include "header.h"

void setup(void){
    EspSerial.begin(ESP8266_BAUD);
    WiFi.init(&EspSerial);
    
    Serial.begin(115200);

    wifiConnect();

    client.setServer(server, 1883);
    client.setCallback(callback);
}

void loop(void){
//파트1
    if ( WiFi.status() != WL_CONNECTED) {
        wifiConnect();
    }
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
}