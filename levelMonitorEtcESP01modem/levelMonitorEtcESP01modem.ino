#include "header.h"

void setup(void){
    Serial.begin(115200);
    EspSerial.begin(ESP8266_BAUD);
    WiFi.init(&EspSerial);

    //debug
    Serial.print("the value of WL_NO_SHIELD is :"); Serial.println(WiFi.status());
    wifiConnect();

    client.setServer(server, 1883);
    client.setCallback(callback);

    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
}

void loop(void){
//파트1
    if (WiFi.status() != WL_CONNECTED) {
        wifiConnect();
    }
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
//파트2
    unsigned long duration, distance;
    //초음파 스피커에 펄스를 준다. 0.01초
    digitalWrite(TRIG, HIGH);
    delay(10);
    digitalWrite(TRIG, LOW);
    //pulsIn함수는 pulsIn(핀번호, 핀상태)의 형태로 이루어져 있으며 핀 상태가 바뀌면 
    //그때까지 경과한 시간을 ms(마이크로 세컨드)단위로 반환시켜줍니다. 
    //다시말해서 pulsIn함수의 핀의 상태값을 HIGH로 지정하면 
    //핀의 상태가 HIGH가 되는 순간부터 음파가 되돌아와 수신되면 Echo핀이 다시 LOW상태가 됩니다. 
    //이때 HIGH 상태에서 LOW상태로 변경되기 까지의 시간을 리턴해주는것 입니다.
    duration = pulseIn(ECHO, HIGH);
    distance = (unsigned long) ((34000*duration)/1000000)/2;

    Serial.print(distance);
    Serial.println("cm");

    delay(100);
}