// B 아두이노 슬레이브 수신
#include <SoftwareSerial.h>

#define ENABLE 10
#define LED 13

// RO(Rx-6번), DI(Tx-7번)
SoftwareSerial rs485(6,7);
void setup() {
  Serial.begin(115200);
  rs485.begin(19200);
  pinMode(ENABLE, OUTPUT);
  pinMode(LED, OUTPUT);
  // RE/DE에서 Write시에 HIGH
  // Read시에 LOW이지만, 여기서는 받기만 한다.
  digitalWrite(ENABLE, LOW);
}

void loop() {
  if(rs485.available()>=1){
    char* val = rs485.read();

   //Add code for HM100
   Serial.println(val);
    if(val=='O'){
      digitalWrite(LED, HIGH);
      Serial.println("LED ON");
    }
    else if(val=='X'){
      digitalWrite(LED, LOW);
      Serial.println("LED OFF");
    }
  }
}
