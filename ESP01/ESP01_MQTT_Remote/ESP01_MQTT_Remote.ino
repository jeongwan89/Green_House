#include <EspMQTTClient.h>

#define SWITCH_PIN 2
#define LED_PIN 0

#define GARDEN
#include "/home/kjw/Git/Green_House/mylibraries/AddrMQTT.h"

const char *ssid = "FarmMain5G";
const char *password = "wweerrtt";
const char *mqtt_server = "192.168.0.24";
const char *mqtt_username = "farmmain";
const char *mqtt_password = "eerrtt";
const char *mqtt_clientname = "Esp01CompRemoteControl";

EspMQTTClient client{
    ssid,
    password,
    mqtt_server,
    mqtt_username,
    mqtt_password,
    mqtt_clientname,
};

void onConnectionEstablished()
{
    client.publish(WILLTOPIC, "online", 1);
    Serial.println("connected");
}

/**
 * 리턴값
 * -1: 바뀌지 않았다.
 * 0: 0으로 바뀌었다.
 * 1: 1로 바뀌었다.
 */
int isChangedTo(int pinState)
{
    static int criteria = -1; // 초기화만 -1로 하고 평소에 갖는 값은 0이거나 1이다.
    if (criteria != pinState)
    {
        Serial.println("Input Changed");
        criteria = pinState;
        return pinState;
    }
    else
    {
        return -1;
    }
}

void setup()
{
    Serial.begin(115200);
    client.enableDebuggingMessages();
    client.enableHTTPWebUpdater();
    client.enableOTA();
    client.enableLastWillMessage(WILLTOPIC, "offline", 1);
    pinMode(SWITCH_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
}

void loop()
{
    int state;
    state = isChangedTo(digitalRead(SWITCH_PIN));
    delay(500);
    if (state == HIGH)
        client.publish(COMP, "1", 1);
    else if (state == LOW)
        client.publish(COMP, "0", 1);

    client.loop();
    if (client.isConnected())
        digitalWrite(LED_PIN, LOW);
    else
        digitalWrite(LED_PIN, HIGH);
}