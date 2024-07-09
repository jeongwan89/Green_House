#ifndef ADDRMQTT
#define ADDRMQTT

#ifdef TM1637MON
#define ESP8266_BAUD 57600
#define MQTTID "Temp_Monitor_Proto"
#define MQTTUSER "farmmain"
#define MQTTPASS "eerrtt"
#define WILLTOPIC "Lastwill/Monitor/Greenhouse/Tempmonitor1"
#define WILLMSG "off line"
#define GH1_TEMP "Sensor/GH1/Center/Temp"
#define GH1_HUM "Sensor/GH1/Center/Hum"
#define GH2_TEMP "Sensor/GH2/Center/Temp"
#define GH2_HUM "Sensor/GH2/Center/Hum"
#define GH3_TEMP "Sensor/GH3/Center/Temp"
#define GH3_HUM "Sensor/GH3/Center/Hum"
#define GH4_TEMP "Sensor/GH4/Center/Temp"
#define GH4_HUM "Sensor/GH4/Center/Hum"
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


#ifdef Esp01_DHT22_MQTT_01
#define CLIENT_NAME "Green_House_Temp_RH_Monitor_01"
#define MQTT_PUB_TEMP "Sensor/GH1/Center/Temp" // GH1 = Green House (온실) 1동
#define MQTT_PUB_HUM "Sensor/GH1/Center/Hum"
#define SENSOR_STATUS "Sensor/GH1/Center/Stat"
#define TEMP_CAL 0
#define HUM_CAL 0
#endif

#ifdef Esp01_DHT22_MQTT_02
#define CLIENT_NAME "Green_House_Temp_RH_Monitor_02"
#define MQTT_PUB_TEMP "Sensor/GH2/Center/Temp"
#define MQTT_PUB_HUM "Sensor/GH2/Center/Hum"
#define SENSOR_STATUS "Sensor/GH2/Center/Stat"
#define TEMP_CAL 0
#define HUM_CAL 0
#endif

#ifdef Esp01_DHT22_MQTT_03
#define CLIENT_NAME "Green_House_Temp_RH_Monitor_03"
#define MQTT_PUB_TEMP "Sensor/GH3/Center/Temp"
#define MQTT_PUB_HUM "Sensor/GH3/Center/Hum"
#define SENSOR_STATUS "Sensor/GH3/Center/Stat"
#define TEMP_CAL 0
#define HUM_CAL 0
#endif

#ifdef Esp01_DHT22_MQTT_04
#define CLIENT_NAME "Green_House_Temp_RH_Monitor_04"
#define MQTT_PUB_TEMP "Sensor/GH4/Center/Temp"
#define MQTT_PUB_HUM "Sensor/GH4/Center/Hum"
#define SENSOR_STATUS "Sensor/GH4/Center/Stat"
#define TEMP_CAL 0
#define HUM_CAL 0
#endif

#ifdef Esp01_DHT22_MQTT_05
#define CLIENT_NAME "NR_House_Temp_RH_Monitor_01"
#define MQTT_PUB_TEMP "Sensor/NR1/Center/Temp" // NR1 = Nursery Green House (육묘장) 1동
#define MQTT_PUB_HUM "Sensor/NR1/Center/Hum"
#define SENSOR_STATUS "Sensor/NR1/Center/Stat"
#define TEMP_CAL 0
#define HUM_CAL 0
#endif

#ifdef ESP01_MQTT_NURSERY_MIST_CONTROL
#define DURA "Argument/NR/mist/dura"
#define PERIOD "Argument/NR/mist/period"
#define MOTOR "Argument/NR/mist/motor"
#define AUTO "Argument/NR/mist/auto"
#define WILLTOPIC "Lastwill/NR/mist/status"
#endif

#ifdef MIST_CONTROL_MQTT
#define MQTTID "mistcontrol"
#define MQTTUSER "farmmain"
#define MQTTPASS "eerrtt"
#define M_FREQ "Argument/WRS/mistcontrol/freq"
#define M_DURA "Argument/WRS/mistcontrol/dura"
#define M_MOTOR "Argument/WRS/mistcontrol/motor"
#define M_GH1 "Argument/WRS/mistcontrol/GH1"
#define M_GH2 "Argument/WRS/mistcontrol/GH2"
#define M_GH3 "Argument/WRS/mistcontrol/GH3"
#define M_GH4 "Argument/WRS/mistcontrol/GH4"
#define M_AUTO "Argument/WRS/mistcontrol/auto"
#define M_STAT_MOTOR "Status/WRS/mistcontrol/motor"
#endif

#ifdef ROOM_TEST
#define TEMP1 "Sensor/GH1/Center/Temp"
#define TEMP2 "Sensor/GH2/Center/Temp"
#define TEMP3 "Sensor/GH3/Center/Temp"
#define TEMP4 "Sensor/GH4/Center/Temp"
#define HUM1 "Sensor/GH1/Center/Hum"
#define HUM2 "Sensor/GH2/Center/Hum"
#define HUM3 "Sensor/GH3/Center/Hum"
#define HUM4 "Sensor/GH4/Center/Hum"
#define WILLTOPIC "Lastwill/GH1/Center/Status"
#endif

#endif