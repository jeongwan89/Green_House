/* BCD pour activer deux micro 74LS48 ou deux digites
on a besoin de 8 output bit*/
include "myBCD.h" ;

void setup(){
  for (i=3 ; i<=10 ; i++){
    pinMode (i, OUTPUT);
  }
}

void loop(){
  for(i=0 ; i < 100 ; i++) {
    printBCD(i);
    delay (1000);
  }

