#include <SoftwareSerial.h>

#define SSerialTxControl 10

#define CHIP485_SEL_TX digitalWrite(SSerialTxControl,HIGH)  //Transmission
#define CHIP485_SEL_RX digitalWrite(SSerialTxControl,LOW)   //Receive

SoftwareSerial rs485(6,7); //RX-RO TX-DI
unsigned char Data[16];   //data type이 unsigned char이어야 함수의 type declaration에 맞더라. 특히 sprintf쪽에서 문제를 일으켰음
byte  requestData[8]={
        0x01,   //Device ID
        0x03,   //Function
        0x00,   //Address high byte
        0x68,   //Address low byte
        0x00,   //Data high byte
        0x03,   //Data low byte
        0x84,   //CRC high byte
        0x17    //CRC low byte
      };     
void DemandData(void)
{
  char mP[5];
  CHIP485_SEL_TX;

  //이후에 SoftwareSerial rs485에 byte requestData 써 넣는 루틴
  //rs485.write(requestData,8);
  //한 바이트씩 보내기
  Serial.print("Demand Data to HM-100 sending:");
  for (int i=0; i<8; i++){
    rs485.write(requestData[i]);
    sprintf(mP, "0x%02x ", requestData[i]);
    Serial.print(mP);
  }
  Serial.println();

/* 시험코드이므로 무시
  rs485.write(0x01);
  rs485.write(0x03);
  rs485.write((char)0x00);
  rs485.write(0x68);
  rs485.write((char)0x00);
  rs485.write(0x03);
  rs485.write(0x17);
  rs485.write(0x84);  
*/
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

void Parsing(void){
  float EC;
  float pH;
  float temp;

  EC = (float)((int)Data[3]*256+(int)Data[4])/100;
  pH = (float)((int)Data[5]*256+(int)Data[6])/100;
  temp = (float)((int)Data[7]*256+(int)Data[8])/10;

  for(int i=0; i<16; i++){
    Data[i] = 0x00;
  }
  //참고로 pH, temp, EC는 hm-100의 단위 select에 따라 달라진다.
  Serial.print("pH = "); Serial.println(pH);
  Serial.print("tp = "); Serial.println(temp);
  Serial.print("EC = "); Serial.println(EC);
}
void setup(){
  pinMode(SSerialTxControl, OUTPUT);
  Serial.begin(115200);
  rs485.begin(19200);     //HM-100통신 규약이 Modbus, baud rate 19200
  CHIP485_SEL_RX;
}

void loop(){
  DemandData();
  delay(500);             //HM-100측에서 데이터를 보내와 버퍼에 쌓이는 충분한 시간 고려
  ReadData();
  Parsing();
  delay(5000);
}
