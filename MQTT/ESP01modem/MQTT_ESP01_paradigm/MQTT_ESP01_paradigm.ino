#include "header.h"

void setup(void){
    Serial.begin(115200);

    wifiConnect();

    client.setServer(server, 1883);
    client.setCallback(callback);
}

void loop(void){
//파트1
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
}