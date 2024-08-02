#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>

#define VR A0
#define PWM 3

#define debug
void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(VR, INPUT);
    pinMode(PWM, OUTPUT);
    Serial.begin(115200);
}

void loop()
{
    static int vrValue;
    vrValue = analogRead(VR);
#ifdef debuf
    Serial.println(vrValue);
#endif
}