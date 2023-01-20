#include <SoftwareSerial.h>
SoftwareSerial ESPSerial(6, 7); //(RX,TX)
void setup() {
  Serial.begin(9600);
  ESPSerial.begin(9600);
}

void loop() {
  while(Serial.available()){
    ESPSerial.write(Serial.read());
  }
  while(ESPSerial.available()){
    Serial.write(ESPSerial.read());
  }
}
