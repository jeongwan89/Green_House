#ifndef ADDRMQTT
#define ADDRMQTT

    #ifdef TM1637MON
    #define ESP8266_BAUD 57600

    #define MQTTID "Temp_Monitor_Proto"
    #define MQTTUSER "farmmain"
    #define MQTTPASS "eerrtt"
    #define WILLTOPIC "Lastwill/Monitor/Greenhouse/Tempmonitor1"
    #define WILLMSG "off line"

    #define CLK_1 2
    #define DIO_1 3
    #define CLK_2 4
    #define DIO_2 5
    #define CLK_3 6
    #define DIO_3 7
    #define CLK_4 8
    #define DIO_4 9
    #define CLK_5 10
    #define DIO_5 11
    #define CLK_6 12
    #define DIO_6 13
    #define CLK_7 14
    #define DIO_7 15
    #define CLK_8 16
    #define DIO_8 17

    #define SOFT_RX 18 // A4
    #define SOFT_TX 19 // A5
    #endif
#endif
