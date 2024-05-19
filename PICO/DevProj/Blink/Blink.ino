#include <stdio.h>
#include <stdlib.h>

#include "header.h"

#define ESP8266_BAUD 115200

// the setup function runs once when you press reset or power the board
void setup()
{
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(2, OUTPUT);
    Serial.begin(115200);
    Serial1.begin(ESP8266_BAUD);
    WiFi.init(&Serial1, 2);
    delay(500);
    Serial.print("Now entering wifiConnect()\n");
    wifiConnect();
}

// the loop function runs over and over again forever
void loop()
{
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
    delay(50);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
    delay(50);                       // wait for a second
}
