#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>

#define debug

#define VR  A0
#define PWM 3
#define DIR 4
#define SW  6
#ifdef  debug
#define PWM_LED 5
#endif

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(VR, INPUT);
    pinMode(PWM, OUTPUT);
    pinMode(DIR, OUTPUT);
    pinMode(SW, INPUT_PULLUP);
#ifdef debug
    pinMode(PWM_LED, OUTPUT);
    Serial.begin(115200);
#endif
    digitalWrite(DIR, LOW);
}

void loop()
{
    static int stateSW = 0;
    static int lastStateSW = 0;
    static bool motorDir = 0;
    int vrValue;
    int vrMapped;
    vrValue = analogRead(VR);
    vrMapped = map(vrValue, 7, 1023, 0, 255);
#ifdef debug
    Serial.println(vrValue);
    analogWrite(PWM_LED, vrMapped);
#endif
    stateSW = digitalRead(SW);
    if(lastStateSW != stateSW)
    {
        delay(100);
        if(stateSW == HIGH) 
        {
            // decrement
            int currVrMapped = vrMapped;
            for(int i=0; i < 256; i++)
            {
                if(currVrMapped == 0) break;
                currVrMapped -- ;
                analogWrite(PWM, currVrMapped);
                delay(10);
            }
            motorDir = !motorDir;
            digitalWrite(DIR, motorDir);
            // increment
            for(int i=0; i < 256; i++)
            {
                if(currVrMapped == vrMapped) break;
                currVrMapped ++ ;
                analogWrite(PWM, currVrMapped);
                delay(10);
            }
        }
        lastStateSW = stateSW;
    }
    analogWrite(PWM, vrMapped);
}