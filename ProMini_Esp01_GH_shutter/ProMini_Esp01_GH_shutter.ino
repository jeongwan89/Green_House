#include <stdlib.h>
#include "shutter_header.h"
#include "shutter.h"
// 루프 밖에서 정의 부분
// W1 (1중 서편), W2 (2중 동편), E1(1중 동편), E2(2중 동편)
extern class Shutter W1, W2, E1, E2; // shutter_header.h에 정의/선언되어 있다.
float W1_curr, W2_curr, E1_curr, E2_curr; // scanMotorState에서 mA단위로 get!

// 주기적으로 실행하는 process에 대한 class 선언
RaiseTimeEventInLoop each5sec;  // 하는 일 : MQTT에 모터 상태 보고
// TODO: 뭔가 할일이 없다. FIXME: 도 할 일이 없다.
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
    pinMode(ESP01_RESET, OUTPUT);

    // ESP01 modem 리세트. 하드웨어로 wiring
    digitalWrite(ESP01_RESET, LOW);
    delay(100);
    digitalWrite(ESP01_RESET, HIGH);
    delay(500);
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
// loop 마다 : 모터상태 확인
// 1초마다 : 모터상태 확인
// 5초마다 : MQTT에 올림
    scanMotorState();
    // each1sec.eachEveryTimeIn(1000, scanMotorState);
    each5sec.eachEveryTimeIn(5000, pubMotorState);
}

/// @brief 모터에 흐르는 전류 값을 MQTT에 publish
// 이 함수는 5초마다 실행하는 루프에 연결되어있다.
/// @param  void
void pubMotorState(void)
{
    char buf[8];
    
    client.publish(MQTT_PUB_E1_CURR, dtostrf(E1_curr, 8, 2, buf));
    delay (100);
    client.publish(MQTT_PUB_E2_CURR, dtostrf(E2_curr, 8, 2, buf));
    delay (100);
    client.publish(MQTT_PUB_W1_CURR, dtostrf(W1_curr, 8, 2, buf));
    delay (100);
    client.publish(MQTT_PUB_W2_CURR, dtostrf(W2_curr, 8, 2, buf));
    delay (100);
}

/// @brief 하우스 모터를 INA219를 통해서 전류와 상태를 스캔한다.
// 만약 3A이상으로 전류가 흐르면 즉시 모터를 정지하고 상태를 MQTT에 올린다.
/// @param  void
void scanMotorState(void)
{
    E1_curr = E1.getCurrent_mA();
    E2_curr = E2.getCurrent_mA();
    W1_curr = W1.getCurrent_mA();
    W2_curr = W2.getCurrent_mA();
    if(E1_curr > 3000) {
        E1.Stop();
        client.publish(MQTT_E1_MTR_CURR_ERROR, "OVER_CURRENT");
    }
    if(E2_curr > 3000) {
        E2.Stop();
        client.publish(MQTT_E2_MTR_CURR_ERROR, "OVER_CURRENT");
    }
    if(W1_curr > 3000) {
        W1.Stop();
        client.publish(MQTT_W1_MTR_CURR_ERROR, "OVER_CURRENT");
    }
    if(W2_curr > 3000) {
        W2.Stop();
        client.publish(MQTT_W2_MTR_CURR_ERROR, "OVER_CURRENT");
    }
}