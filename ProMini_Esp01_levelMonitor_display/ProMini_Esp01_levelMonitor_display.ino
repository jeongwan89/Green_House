#include "header.h"

RaiseTimeEventInLoop mqttPub;

void setup(void){
    Serial.begin(115200);
    EspSerial.begin(ESP8266_BAUD);
    WiFi.init(&EspSerial);
    display.setBrightness(0x07);
    display.showNumberDec(0, true);
    //debug
    //Serial.print("the value of WL_NO_SHIELD is :"); Serial.println(WiFi.status());
    wifiConnect();

    client.setServer(server, 1883);
    client.setCallback(callback);

    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LEVEL_LIMIT, INPUT_PULLUP);
    pinMode(ERR_RELAY, OUTPUT);
}

void loop(void)
{
    static unsigned long lastPub = millis();
    static bool limitStatChanged = false;
    static bool preLimitStat = false;
    static bool currLimitStat = false;
    unsigned long now;
    int averageDistance;
    char buffer[128];

// 파트1 ===========================================================================================
// client.loop()를 먼저한 이유는 현재 loop() 속에서 mqtt 관련 함수를 먼저 refresh하기 위해서이다.
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

// 파트2 ===========================================================================================
// loop()동안 쉴새 없이 거리를 계속 업데이트 한다.
    averageDistance = avrDistance(ultraSonicMeasureDistance(TRIG, ECHO));
    display.showNumberDec(volumeRemain(averageDistance), false);
    isChangedDoSomething(& preLimitStat, & currLimitStat);
// 파트3 : MQTT에 publish하는 프로그램이다. ========================================================
//  정해진 시간에 한번씩 쓰는 것인데, 14초에 하나씩 쓰도록 한다.
//  만들어 놓은 class를 이용하여 간단하게 표현한다.
//  14초 동안 읽은 거리는 평균을 내서 이용한다.
    mqttPub.eachEveryTimeIn(14000, doThis, MQTT_PUB_LEVEL, volumeRemain(averageDistance));
}