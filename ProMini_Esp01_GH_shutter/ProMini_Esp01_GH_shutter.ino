#include "shutter_header.h"
#include "shutter.h"
// 루프 밖에서 정의 부분
// W1 (1중 서편), W2 (2중 동편), E1(1중 동편), E2(2중 동편)
Shutter W1(2, 3, 0x40), W2(4, 5, 0x41), E1(6, 7, 0x43), E2(8, 9, 0x44);

// 주기적으로 실행하는 process에 대한 class 선언
RaiseTimeEventInLoop each5sec;  // 하는 일 : MQTT에 모터 상태 보고
RaiseTimeEventInLoop each1sec;  // 하는 일 : 온실 모터 상태 확인
void setup(void)
{
    Serial.begin(115200);   // for debug
    EspSerial.begin(ESP8266_BAUD);
    WiFi.init(&EspSerial);
    wifiConnect();

    client.setServer(server, 1883);
    client.setCallback(callback);

    pinMode(LED_BUILTIN, OUTPUT); 
}

void loop(void)
{
// 파트1 ========================================================================
// client.loop()를 먼저한 이유는 현재 loop() 속에서 mqtt 관련 함수를 먼저 refresh하기 위해서이다.
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

// 파트2=========================================================================
// 5초마다 : MQTT에 올림
// 1초마다 : 모터상태 확인
    each5sec.eachEveryTimeIn(5000, pubMotorState);
    each1sec.eachEveryTimeIn(1000, scanMotorState);
}