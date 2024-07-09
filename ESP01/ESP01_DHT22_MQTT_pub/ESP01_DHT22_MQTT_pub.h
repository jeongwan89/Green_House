#include <EspMQTTClient.h>

const char * ssid = "FarmMain5G";
const char * password = "wweerrtt";
const char * mqtt_server = "192.168.0.24";
const char * mqtt_username = "farmmain";
const char * mqtt_password = "eerrtt";
const char * mqtt_clientname = "Esp01LabMonitorInRoom"; 

EspMQTTClient client(
    ssid,
    password,
    mqtt_server,
    mqtt_username,
    mqtt_clientname
);

char message[128];

void (*reset)(void) = 0;