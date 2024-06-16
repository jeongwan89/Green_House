#ifndef ADDRMQTT
#define ADDRMQTT

    #ifdef TM1637MON
    #define ESP8266_BAUD 57600

    #define MQTTID "Temp_Monitor_Proto"
    #define MQTTUSER "farmmain"
    #define MQTTPASS "eerrtt"
    #define WILLTOPIC "Lastwill/Monitor/Greenhouse/Tempmonitor1"
    #define WILLMSG "off line"
    #undef TM1637Mon
    #endif


    #ifdef GH1_WINDOW
    #define CLIENT_NAME "Green_House_Window_Shutter_01"
    #define MQTT_ACT_1_E "Motor/GH1/1/E" // GH1 = Green House (온실) 1동
    #define MQTT_ACT_1_W "Motor/GH1/1/W"
    #define MQTT_ACT_2_E "Motor/GH1/2/E"
    #define MQTT_ACT_2_W "Motor/GH1/2/W"
    #define WILLTOPIC "Lastwill/Motor/GH1/Status"
    #undef GH1_WINDOW
    #endif

    #ifdef GH2_WINDOW
    #define CLIENT_NAME "Green_House_Window_Shutter_02"
    #define MQTT_ACT_1_E "Motor/GH2/1/E"
    #define MQTT_ACT_1_W "Motor/GH2/1/W"
    #define MQTT_ACT_2_E "Motor/GH2/2/E"
    #define MQTT_ACT_2_W "Motor/GH2/2/W"
    #define WILLTOPIC "Lastwill/Motor/GH2/Status"
    #undef GH2_WINDOWS
    #endif

    #ifdef GH3_WINDOW
    #define CLIENT_NAME "Green_House_Window_Shutter_03"
    #define MQTT_ACT_1_E "Motor/GH3/1/E"
    #define MQTT_ACT_1_W "Motor/GH3/1/W"
    #define MQTT_ACT_2_E "Motor/GH3/2/E"
    #define MQTT_ACT_2_W "Motor/GH3/2/W"
    #define WILLTOPIC "Lastwill/Motor/GH3/Status"
    #undef GH3_WINDOWS
    #endif

    #ifdef GH4_WINDOW
    #define CLIENT_NAME "Green_House_Window_Shutter_04"
    #define MQTT_ACT_1_E "Motor/GH4/1/E"
    #define MQTT_ACT_1_W "Motor/GH4/1/W"
    #define MQTT_ACT_2_E "Motor/GH4/2/E"
    #define MQTT_ACT_2_W "Motor/GH4/2/W"
    #define WILLTOPIC "Lastwill/Motor/GH4/Status"
    #undef GH4_WINDOWS
    #endif

    #ifdef NR1_WINDOW
    #define CLIENT_NAME "NR_House_Window_Shutter_01"
    #define MQTT_ACT_1_E "Motor/NR1/1/E" // NR1 = Nursery Green House (육묘장) 1동
    #define MQTT_ACT_1_W "Motor/NR1/1/W"
    #define MQTT_ACT_2_E "Motor/NR1/2/E"
    #define MQTT_ACT_2_W "Motor/NR1/2/W"
    #define WILLTOPIC "Lastwill/Motor/NR1/Status"
    #undef NR1_WINDOW
    #endif

    #ifdef GH1_ENV
    #define CLIENT_NAME "GH1_ENV"
    #define TPC_AIRTEMP "Sensor/GH1/Center/AirTemp"
    #define TPC_AIRHUM "Sensor/GH1/Center/AirHum"
    #define WILLTOPIC "Lastwill/GH1/Center/Status"
    #undef GH1_ENV
    #endif
#endif
