#pragma once
#ifndef SOURCE_H_
#define SOURCE_H_
#include <EspMQTTClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define ROOM_TEST
#include "/home/kjw/Git/Green_House/mylibraries/AddrMQTT.h"

#define DHTPIN 2
#define DHTTYPE DHT22
DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

const char *ssid = "FarmMain5G";
const char *password = "wweerrtt";
const char *mqtt_server = "192.168.0.24";
const char *mqtt_username = "farmmain";
const char *mqtt_password = "eerrtt";
const char *mqtt_clientname = "Esp01LabMonitorInRoom";

EspMQTTClient client(
    ssid,
    password,
    mqtt_server,
    mqtt_username,
    mqtt_password,
    mqtt_clientname,
    1883);

#include "/home/kjw/Git/Green_House/mylibraries/RaiseEventClass.h"
RaiseTimeEventInLoop RaiseEvent_5s;

char message[128];

void (*resetFunc)(void) = 0;

// 이 callback 함수는 EspMQTTClient.h에 정의 되어 있다.
// include된 순서가 중요하겠지?
void onConnectionEstablished()
{
    //   // Subscribe to "mytopic/test" and display received message to Serial
    //   client.subscribe("mytopic/test", [](const String & payload) {
    //     Serial.println(payload);
    //   });

    //   // Subscribe to "mytopic/wildcardtest/#" and display received message to Serial
    //   client.subscribe("mytopic/wildcardtest/#", [](const String & topic, const String & payload) {
    //     Serial.println("(From wildcard) topic: " + topic + ", payload: " + payload);
    //   });

    // Publish a message to "mytopic/test"
    client.publish(WILLTOPIC, "online", 1); // You can activate the retain flag by setting the third parameter to true

    // Execute delayed instructions
    //   client.executeDelayed(5 * 1000, []() {
    //     client.publish("mytopic/wildcardtest/test123", "This is a message sent 5 seconds later");
    //  });
}

void pubTempHum(void)
{
    sensors_event_t event;
    static float temp = 0;
    static float hum = 0;
    char payload [7];

    dht.temperature().getEvent(&event);
    if(temp != event.temperature)
    {
        temp = event.temperature;
        snprintf(payload, 6, "%.1f", temp);
        client.publish(TEMP1, payload, 1);
        client.publish(TEMP2, payload, 1);
        client.publish(TEMP3, payload, 1);
        client.publish(TEMP4, payload, 1);
    }

    dht.humidity().getEvent(&event);
    if(hum != event.relative_humidity)
    {
        hum = event.relative_humidity;
        snprintf(payload, 6, "%.1f", hum);
        client.publish(HUM1, payload, 1);
        client.publish(HUM2, payload, 1);
        client.publish(HUM3, payload, 1);
        client.publish(HUM4, payload, 1);
    }
}

#endif