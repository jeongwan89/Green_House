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

#include "/home/kjw/Git/Green_House/mylibraries/RaiseEventClass.h"
RaiseTimeEventInLoop getTime;

// #include <WiFiEspAT.h>
// #include <PubSubClient.h>
// #include <SoftwareSerial.h>
// #define PROMINI_7SEG_RCT
// #include "/home/kjw/Git/Green_House/mylibraries/AddrMQTT.h"
// #define SOFT_RX 18
// #define SOFT_TX 19
// #define ESP8266_BAUD 57600
// char ssid[] = "FarmMain5G";
// char pass[] = "wweerrtt";
// int status = WL_IDLE_STATUS;

// IPAddress server(192, 168, 0, 24);

// SoftwareSerial EspSerial(SOFT_RX, SOFT_TX); // Rx, Tx
// WiFiClient espClient;
// PubSubClient client(espClient);

// resetFunction
void (*resetFunc)(void) = 0; // 리셋함수는 어드레스가 0부터 시작하는 함수이다.
void (*showTIMEInSevSeg)(void);

/*
    이 콜백에서는 따로 payload뿐만 아니라 str[256]에서 카피를 받아쓴다.
    payload마지막에 NULL이 있는지 없는지는 모르겠지만,
    str에는 마지막에 NULL이 있어서 string으로 쓸수 있다.
*/
// void callback(char *topic, byte *payload, unsigned int length)
// {
//     char str[128];
//     int conv;
//     Serial.print("Message arrived [");
//     Serial.print(topic);
//     Serial.print("] : ");
//     for (int i = 0; i < length; i++)
//     {
//         Serial.print((char)payload[i]);
//         str[i] = (char)payload[i];
//         str[i + 1] = '\0';
//     }
//     Serial.println();

//     double value = atof(str);
//     int ivalue = (value * 10);

//     // 이하 특별한 topic message에 대한 처리 루틴이 있어야한다.
//     if (strcmp(topic, SHOWTIME) == 0)
//     {
//         if (strcmp(str, "hour") == 0)
//             showTIMEInSevSeg = showHourInSevSeg;
//         else if (strcmp(str, "min") == 0)
//             showTIMEInSevSeg = showMinInSevSeg;
//     }
// }

// void reconnect(void)
// {
//     int errCnt = 0;
//     while (!client.connected())
//     {
//         Serial.print("Attempting MQTT conncection...");
//         if (client.connect(MQTTID, MQTTUSER, MQTTPASS, WILLTOPIC, 0, 1, WILLMSG))
//         {
//             Serial.print("connected!");
//             client.publish(WILLTOPIC, "on line", 1);
//             client.publish(SHOWTIME, "hour", 1);

//             // client.subscribe("...")
//             client.subscribe(SHOWTIME);
//         }
//         else
//         {
//             Serial.print("Failed, rc=");
//             Serial.print(client.state());
//             Serial.print("\t try again in 5 second\n");
//             delay(5000);
//             if (errCnt > 5)
//                 resetFunc();
//             else
//                 errCnt++;
//         }
//     }
// }

// void wifiConnect(void)
// {
//     /* setup() 안에서 작동한다. 이전에 반드시 정의 되어야할 것이 있는데,
//         ESP01이 SoftwareSerial로 정의되어 있어야 하고(EspSerial),
//         WiFi 쉴드 변수와 ESP01모뎀의 객체의 레퍼런스를 연결해 주어야 한다.
//     */
//     if (WiFi.status() == WL_NO_SHIELD)
//     {
//         Serial.println("WiFi shield not present");
//         delay(100);
//         resetFunc();
//     }

//     Serial.print("Attempting to connect to WPA SSID:");
//     Serial.println(ssid);

//     WiFi.begin(ssid, pass);

//     int errCnt = 0;
//     while (WiFi.status() != WL_CONNECTED)
//     {
//         if (errCnt > 10)
//         {
//             resetFunc();
//         }
//         errCnt++;
//         delay(500);
//         Serial.print(".");
//     }

//     Serial.println("Your're connected to the networtk!");
//     Serial.print("IP Address: ");
//     Serial.println(WiFi.localIP());
// }

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

void showHourInSevSeg()
{
    int time;
    time = now.hour * 100;
    time += now.minute;
    sevseg.setNumber(time, 2);
}

void showMinInSevSeg()
{
    int time;
    time = now.minute * 100;
    time += now.second;
    sevseg.setNumber(time, 2);
}

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
    // EspSerial.begin(ESP8266_BAUD);
    // WiFi.init(&EspSerial);
    // wifiConnect();
    // client.setServer(server, 1883);
    // client.setCallback(callback);

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
    showTIMEInSevSeg = showHourInSevSeg;
}

void loop()
{
    // 파트1 =======================================================================
    // if (!client.connected())
    // {
    //     reconnect();
    // }
    // client.loop();
    // 파트2 =======================================================================
    static unsigned long timer = millis();
    static int centiSeconds = 0;

    // get the current time
    getTime.EachEveryTimeIn(1000, displayTimeInSerial);
    showTIMEInSevSeg();
    sevseg.refreshDisplay(); // Must run repeatedly
}
