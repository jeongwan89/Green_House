#include <SoftwareSerial.h>

#define SSerialTxControl 10

#define CHIP485_SEL_TX digitalWrite(SSerialTxControl,HIGH)  //Transmission
#define CHIP485_SEL_RX digitalWrite(SSerialTxControl,LOW)   //Receive

SoftwareSerial rs485(6,7); //RX-RO TX-DI
unsigned char Data[16];
byte  requestData[9]={
        0x01,   //Device ID
        0x03,   //Function
        0x00,   //Address high byte
        0x68,   //Address low byte
        0x00,   //Data high byte
        0x03,   //Data low byte
        0x17,   //CRC high byte
        0x84    //CRC low byte
      };     
void DemandData(void)
{
  char mP[5];
  CHIP485_SEL_TX;

  //이후에 SoftwareSerial rs485에 byte requestData 써 넣는 루틴
  //rs485.write(requestData,8);
  //한 바이트씩 보내기
  for (int i=0; i<8; i++){
   rs485.write(requestData[i]);
  }

  Serial.print("Demand Data to HM-100 sending:");
  for(int i=0; i<8; i++){
    sprintf(mP, "0x%02x ", requestData[i]);
    Serial.print(mP);
  }
  Serial.println();

  CHIP485_SEL_RX;
}

int ReadData(void){
  int index =0;
  char mP[5];
  CHIP485_SEL_RX;

  index = rs485.available();
  if(index >0){
    Serial.print("Received Data:");
    for(int i=0; i<index; i++){
       Data[i] = rs485.read();
       sprintf(mP, "0x%02x ", Data[i]);
       Serial.print(mP);
    }
    Serial.print("\t"); Serial.println(index);
  }
  CHIP485_SEL_RX;
  return index;
}

void setup(){
  pinMode(SSerialTxControl, OUTPUT);
  Serial.begin(115200);
  rs485.begin(19200);
  CHIP485_SEL_RX;
}

void loop(){
  DemandData();
  delay(1000);
  ReadData();
  delay(1000);
}
