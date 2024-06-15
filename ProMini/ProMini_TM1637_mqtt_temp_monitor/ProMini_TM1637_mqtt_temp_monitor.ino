#include "ProMini_TM1637_mqtt_temp_monitor.h"

TM1637Display display1(CLK_1, DIO_1);
TM1637Display display2(CLK_2, DIO_2);
TM1637Display display3(CLK_3, DIO_3);
TM1637Display display4(CLK_4, DIO_4);
TM1637Display display5(CLK_5, DIO_5);
TM1637Display display6(CLK_6, DIO_6);
TM1637Display display7(CLK_7, DIO_7);
TM1637Display display8(CLK_8, DIO_8);

void setup()
{
    display1.setBrightness(0x07);
    display2.setBrightness(0x07);
    display3.setBrightness(0x07);
    display4.setBrightness(0x07);
    display5.setBrightness(0x07);
    display6.setBrightness(0x07);
    display7.setBrightness(0x07);
    display8.setBrightness(0x07);

    display1.showNumberDec(0, false);
    display2.showNumberDec(0, false);
    display3.showNumberDec(0, false);
    display4.showNumberDec(0, false);
    display5.showNumberDec(0, false);
    display6.showNumberDec(0, false);
    display7.showNumberDec(0, false);
    display8.showNumberDec(0, false);

    Serial.begin(115200);
    EspSerial.begin(ESP8266_BAUD);
    WiFi.init(&EspSerial);

    wifiConnect();

    client.setServer(server, 1883);
    client.setCallback(callback);
}

void loop()
{
    if(!client.connected()) {
        reconnect();
    }
    client.loop();
}