#include <SoftwareSerial.h>

// LDR (Light)------------------------------------------------------------------
#define cdsPin10  1      //analog pin 1
#define cdsPin05  0
int light10 = 0;
int light05 = 0;
int a;
int b;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  a = analogRead(cdsPin10);
  b = analogRead(cdsPin05);
  light10 = map(a, 0, 1023, 0, 100); //LDRDark:0  ==> light 100%  
  light05 = map(b, 0, 1023, 0, 100); //LDRDark:0  ==> light 100%  
  
  Serial.print("CDS 10mm:"); Serial.print("\t"); Serial.print(a); Serial.print("\t"); Serial.println(light10);
  Serial.print("CDS 05mm:"); Serial.print("\t"); Serial.print(b); Serial.print("\t"); Serial.println(light05);
  Serial.println("");
}
