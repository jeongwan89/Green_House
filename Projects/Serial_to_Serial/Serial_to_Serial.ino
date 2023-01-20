#include <SoftwareSerial.h>
SoftwareSerial ESPSerial(6, 7); //(RX,TX)
void setup() {
  Serial.begin(9600);
  ESPSerial.begin(9600);
}
//int count=0;
void loop() {
  while(Serial.available()){
    ESPSerial.write(Serial.read());
  }
  while(ESPSerial.available()){
    Serial.write(ESPSerial.read());
  }
  //Serial.println(count);
  //count += 1;
}
