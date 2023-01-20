/********* Read Sensors value *************/

void readSensors(void)
{ 
  airTemp = dht.readTemperature() + calib_airTemp;
  airHum = dht.readHumidity()+ calib_airHum;

  DS18B20.requestTemperatures(); 
  soilTemp = DS18B20.getTempCByIndex(0) + calib_soilTemp ; // Sensor 0 will capture Soil Temp in Celcius 
                                              // 보정값 1.5도
  analight = analogRead(ldrPIN);            
  light = map(analight, 0, 1023, 0, 100); //LDRDark:0  ==> dark 100%  

  
  /*
  토양수분을을 비선형으로 하기로 하였으니 아래 선형 코드는 무시
  analHum = analogRead(soilHumPIN);
  soilHum = map(analHum, 660, 300, 0, 100);  //건조할때 660, 물속일때 300
  */
  
  //토양수분율의 비선형 조사표. 이 결정은 센서버저 1.2 기준으로 작성함
  // analHum == 1000, soilHum=0, ~ analHum == 850, soilHum = 10
  // analHum == 850, soilHum=10, ~analHum == 525, soilHum = 90
  // analHum == 525, soilHum=90, ~analHum == 450, soilHum = 100
  analHum = analogRead(soilHumPIN);
  if(analHum > humA2) 
    soilHum = map(analHum, 1000, humA2, 0, 10);
  else if (analHum > humA7)
    soilHum = map(analHum, humA2, humA7, 10, 90);
  else
    soilHum = map(analHum, humA7, 450, 90, 100);

  
  HD_VPD = VPD((float)airTemp, (float)airHum);
}


//VPD 수분부족분 계산 온도와 상대습도를 기준으로------------------------------
String VPD(float temp, float rh){
  float es;
  float HD; //humidity deficience
  String sVPD;

  es = 6.11*pow(10,(7.5*temp/(237.3+temp)));
  HD = es*(1-rh/100);
  sVPD=String(HD+0.5,1);

  return sVPD;
}
