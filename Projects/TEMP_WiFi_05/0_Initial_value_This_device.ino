#define ssid "FarmMain5G"
#define password "wweerrtt"

#define WIFI_SSID   "KT_GiGA_08C8"
#define WIFI_PW     "cf05zc0562"
#define MQTT_BROKER_ADDR  "172.30.1.28"
#define MQTT_BROKER_PORT  1883
#define MQTT_ID   "farmmain5g"
#define MQTT_PW   "eerrtt"
#define MQTT_ID   "NRSRY"
int status = WL_IDLE_STATUS;   // the Wifi radio's status
char publish_msg[16];

//온도 습도 보정값 ------------------------------------------------------
#define calib_soilTemp 0
#define calib_airTemp 0
#define calib_airHum 0

//토양센서 보정값
//2cm 물 깊이에서의 값 humA2
//7cm 물 깊이에서의 값 humA7
#define humA2 850
#define humA7 525

//Test situation
#define DURATION 1000
