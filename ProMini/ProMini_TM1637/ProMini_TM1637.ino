#include <stdlib.h>
#include <stdio.h>

#include <Arduino.h>
#include <TM1637Display.h>

#define CLK_1 2
#define DIO_1 3
#define CLK_2 4
#define DIO_2 5

TM1637Display display1(CLK_1, DIO_1);
TM1637Display display2(CLK_2, DIO_2);
void blink()
{
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
}
void setup()
{
    pinMode(DIO_1, INPUT_PULLUP);
    pinMode(LED_BUILTIN, OUTPUT);
    display1.setBrightness(0x07);
    display2.setBrightness(0x07);
    display1.showNumberDec(0, false);
    display2.showNumberDec(0, false);
    display1.clear();
    display2.clear();
    Serial.begin(115200);
}

void loop()
{
    static int i = 0;
    Serial.println(i);
    display1.showNumberDec(i, false);
    display2.showNumberDec(i, false);
    delay(500);
    blink();
    i++;
}