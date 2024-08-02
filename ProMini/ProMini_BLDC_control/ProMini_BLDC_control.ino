#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>

#define debug

#define VR A0
#define PWM 3
#ifdef debug
#define PWM_LED 5
#endif

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(VR, INPUT);
    pinMode(PWM, OUTPUT);
#ifdef debug
    pinMode(PWM_LED, OUTPUT);
    Serial.begin(115200);
#endif
}

void loop()
{
    int vrValue;
    int vrMapped;
    vrValue = analogRead(VR);
    vrMapped = map(vrValue, 7, 1023, 0, 255);
#ifdef debug
    Serial.println(vrValue);
    analogWrite(PWM_LED, vrMapped);
#endif
    analogWrite(PWM, vrMapped);
}