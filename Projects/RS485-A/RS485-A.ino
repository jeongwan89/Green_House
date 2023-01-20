// A 아두이노 마스터 송신
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
  // Read시에 LOW이지만, 여기서는 보내기만 한다.
  digitalWrite(ENABLE, HIGH);
}

void loop() {
  rs485.print('O');
  Serial.println('O');
  digitalWrite(LED, HIGH);
  delay(1000);
  rs485.print('X');
  Serial.println('X');
  digitalWrite(LED,LOW);
  delay(1000);
}
