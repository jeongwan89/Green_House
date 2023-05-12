#include <WiFiEspAT.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

#include <stdlib.h>

#define ESP8266_BAUD 57600

char ssid[] = "FarmMain5G";
char pass[] = "wweerrtt";
int status = WL_IDLE_STATUS;

IPAddress server(192, 168, 0, 24);

SoftwareSerial EspSerial(8, 7); //Rx, Tx
WiFiClient espClient;
PubSubClient client(espClient);

#define MQTTID "levelMonitor"
#define MQTTUSER "farmmain"
#define MQTTPASS "eerrtt"

#define TRIG 2
#define ECHO 3
