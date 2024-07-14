/* SevSeg Counter Example

 Copyright 2020 Dean Reading

 This example demonstrates a very simple use of the SevSeg library with a 4
 digit display. It displays a counter that counts up, showing deci-seconds.
 */

#include <SevSeg.h>
SevSeg sevseg; // Instantiate a seven segment controller object

#include <Ds1302.h>
// DS1302 RTC instance
Ds1302 rtc(15, 16, 17); //(PIN_ENA, PIN_CLK, PIN_DAT);
Ds1302::DateTime now;
const static char *WeekDays[] =
    {
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday",
        "Sunday"};

#include "mylibraries/RaiseEventClass.h"
RaiseTimeEventInLoop getTime;

#include <
void setup()
{
    byte numDigits = 4;
    byte digitPins[] = {2, 3, 4, 5};
    byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};
    bool resistorsOnSegments = true;      // 'false' means resistors are on digit pins
    byte hardwareConfig = COMMON_CATHODE; // See README.md for options
    bool updateWithDelays = false;        // Default 'false' is Recommended
    bool leadingZeros = false;            // Use 'true' if you'd like to keep the leading zeros
    bool disableDecPoint = false;         // Use 'true' if your decimal point doesn't exist or isn't connected

    Serial.begin(115200);

    sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
                 updateWithDelays, leadingZeros, disableDecPoint);
    sevseg.setBrightness(90);

    rtc.init();
    // test if clock is halted and set a date-time (see example 2) to start it
    if (rtc.isHalted())
    {
        Serial.println("RTC is halted. Setting time...");

        Ds1302::DateTime dt = {
            .year = 24,
            .month = Ds1302::MONTH_JUL,
            .day = 14,
            .hour = 7,
            .minute = 51,
            .second = 30,
            .dow = Ds1302::DOW_SUN};
        rtc.setDateTime(&dt);
    }
}

void displayTimeInSerial()
{
    rtc.getDateTime(&now);

    static uint8_t last_second = 0;
    if (last_second != now.second)
    {
        last_second = now.second;

        Serial.print("20");
        Serial.print(now.year); // 00-99
        Serial.print('-');
        if (now.month < 10)
            Serial.print('0');
        Serial.print(now.month); // 01-12
        Serial.print('-');
        if (now.day < 10)
            Serial.print('0');
        Serial.print(now.day); // 01-31
        Serial.print(' ');
        // Serial.print(WeekDays[now.dow - 1]); // 1-7
        // Serial.print(' ');
        if (now.hour < 10)
            Serial.print('0');
        Serial.print(now.hour); // 00-23
        Serial.print(':');
        if (now.minute < 10)
            Serial.print('0');
        Serial.print(now.minute); // 00-59
        Serial.print(':');
        if (now.second < 10)
            Serial.print('0');
        Serial.print(now.second); // 00-59
        Serial.println();
    }
}

void showTimeInSevSeg()
{
    int time;
    time = now.hour * 100;
    time += now.minute;
    sevseg.setNumber(time, 2);
}

void loop()
{
    static unsigned long timer = millis();
    static int centiSeconds = 0;

    // get the current time
    getTime.EachEveryTimeIn(1000, displayTimeInSerial);
    showTimeInSevSeg();
    sevseg.refreshDisplay(); // Must run repeatedly
}
