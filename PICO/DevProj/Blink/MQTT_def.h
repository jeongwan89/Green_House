/*
  SimpleMQTTClient.ino
  The purpose of this exemple is to illustrate a simple handling of MQTT and Wifi connection.
  Once it connects successfully to a Wifi network and a MQTT broker, it subscribe to a topic and send a message to it.
  It will also send a message delayed 5 seconds later.

  Esp_MQTT_Client_DHT22.ino
  (simpleMQTTClient.ino에서 코드를 승계하였음)
  이 코드의 목적은 DHT22에서 온도를 읽고
  그 온도 값을 MQTT서버에 올리는 일이다.
*/
/*
  아래에서 MQTT Topic의 구조를 유지한다.
  Sensor/Topos1/Topos2/Name/Stat
  Actuator/Topos1/Topos2/Name/Act
 */

/*
  #define구문을 정의한다.
  동마다 위치마다 MQTT의 publish가 다르기 때문에 여기에 한번에 걸쳐서 MQTT의 구조를 유지한다.
  장점으로는 파일을 여러개 만들지 않아도 된다.
  단점으로는 컴파일할 때 define의 정의를 명백히 고쳐서 컴파일 해야한다.
  define의 정의를 고쳐서 컴파일하는 일을 컴파일 인자를 고쳐 컴파일한다고 하겠다.
  #define구문의 정의는 Esp01_DHT22_MQTT_01 ~ xx (100개)로 하고 각 define의 인자를 정의하는 것으로 한다.
*/
#include "Arduino.h"

#define MQTTUSER "farmmain"
#define MQTTPASS "eerrtt"
#define WILLMSG "offLine"

#define GH1_ENV

#ifdef GH1_WINDOW
#define CLIENT_NAME "Green_House_Window_Shutter_01"
#define MQTT_ACT_1_E "Motor/GH1/1/E" // GH1 = Green House (온실) 1동
#define MQTT_ACT_1_W "Motor/GH1/1/W"
#define MQTT_ACT_2_E "Motor/GH1/2/E"
#define MQTT_ACT_2_W "Motor/GH1/2/W"
#define WILLTOPIC "Lastwill/Motor/GH1/Status"
#endif

#ifdef GH2_WINDOW
#define CLIENT_NAME "Green_House_Window_Shutter_02"
#define MQTT_ACT_1_E "Motor/GH2/1/E"
#define MQTT_ACT_1_W "Motor/GH2/1/W"
#define MQTT_ACT_2_E "Motor/GH2/2/E"
#define MQTT_ACT_2_W "Motor/GH2/2/W"
#define WILLTOPIC "Lastwill/Motor/GH2/Status"
#endif

#ifdef GH3_WINDOW
#define CLIENT_NAME "Green_House_Window_Shutter_03"
#define MQTT_ACT_1_E "Motor/GH3/1/E"
#define MQTT_ACT_1_W "Motor/GH3/1/W"
#define MQTT_ACT_2_E "Motor/GH3/2/E"
#define MQTT_ACT_2_W "Motor/GH3/2/W"
#define WILLTOPIC "Lastwill/Motor/GH3/Status"
#endif

#ifdef GH4_WINDOW
#define CLIENT_NAME "Green_House_Window_Shutter_04"
#define MQTT_ACT_1_E "Motor/GH4/1/E"
#define MQTT_ACT_1_W "Motor/GH4/1/W"
#define MQTT_ACT_2_E "Motor/GH4/2/E"
#define MQTT_ACT_2_W "Motor/GH4/2/W"
#define WILLTOPIC "Lastwill/Motor/GH4/Status"
#endif

#ifdef NR1_WINDOW
#define CLIENT_NAME "NR_House_Window_Shutter_01"
#define MQTT_ACT_1_E "Motor/NR1/1/E" // NR1 = Nursery Green House (육묘장) 1동
#define MQTT_ACT_1_W "Motor/NR1/1/W"
#define MQTT_ACT_2_E "Motor/NR1/2/E"
#define MQTT_ACT_2_W "Motor/NR1/2/W"
#define WILLTOPIC "Lastwill/Motor/NR1/Status"
#endif

#ifdef GH1_ENV
#define CLIENT_NAME "GH1_ENV"
#define TPC_AIRTEMP "Sensor/GH1/Center/AirTemp"
#define TPC_AIRHUM "Sensor/GH1/Center/AirHum"
#define WILLTOPIC "Lastwill/GH1/Center/Status"
#endif
