 //함수 정의부분-----------------------------------------------------------
boolean sendATcommand(char * command, char * response, int second) {
  //최대 대기 시간 설정
  ESPSerial.setTimeout(second*1000);
  //AT명령을 ESP-01모듈로 전달하여 실행
  ESPSerial.println(command);
  Serial.println(command);
  //응답대기
  boolean responseFound = ESPSerial.find(response);
  //최대 대기 시간을 default값인 1초로 재설정
  ESPSerial.setTimeout(1000);

  return responseFound;
}

boolean connectWifi(){
  ESPSerial.setTimeout(10*1000);
  String cmd="AT+CWJAP=\"";
         cmd+=ssid;
         cmd+="\",\"";
         cmd+=password;
         cmd+="\"";
  ESPSerial.println(cmd);       
  Serial.println(cmd);

  boolean responseFound = ESPSerial.find("WIFI CONNECTED");
  ESPSerial.setTimeout(1000);

  return responseFound;
}


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


/********* Connection com TCP com Thingspeak *******/
void writeThingSpeak(void)
{
  startThingSpeakCmd();

// preparacao da string GET
  String getStr = "GET /update?api_key=";
  getStr += statusChWriteKey;
//  getStr +="&field1=";
//  getStr += String(pump);
  getStr +="&field2=";
  getStr += String(CO2pwm);
  getStr +="&field3=";
  getStr += String(airTemp);
  getStr +="&field4=";
  getStr += String(airHum);
  getStr +="&field5=";
  getStr += String(soilTemp);
  getStr +="&field6=";
  getStr += String(averagesoilHum[0]); //soilHum대신에 평균값을 넣었음
  getStr +="&field7=";
  getStr += String(light);
  getStr +="&field8=";
  getStr += String(HD_VPD);
  getStr += "\r\n\r\n";

  //debug
  //Serial.print("debug:"); Serial.print(getStr);
  //debug
  sendThingSpeakGetCmd(getStr); 
}


/********* Reset ESP *************/
void EspHardwareReset(void)
{
  Serial.println("ESP-01 HARDWARE Reseting......."); 
  digitalWrite(HARDWARE_RESET, LOW); 
  delay(500);
  digitalWrite(HARDWARE_RESET, HIGH);
  delay(8000);//읽기 시작에 필요한 시간 
  Serial.println("RESET"); 
}


/********* Start communication with ThingSpeak*************/
void startThingSpeakCmd(void)
{
  ESPSerial.flush();//쓰기를 시작하기 전에 버퍼를 지웁니다
  
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // IP api.thingspeak.com
  cmd += "\",80";
  ESPSerial.println(cmd);
  Serial.print("send ==> Start cmd: ");
  
  Serial.println(cmd);

  if(ESPSerial.find("Error"))
  {
    Serial.println("AT+CIPSTART error");
    return;
  }
}
/********* send a GET cmd to ThingSpeak *************/
String sendThingSpeakGetCmd(String getStr)
{
  String cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
//debug
//  Serial.print("debug:");   Serial.println(getStr);
//debug
  ESPSerial.println(cmd);
  Serial.print("send ==> lenght cmd: ");
  Serial.println(cmd);

  if(ESPSerial.find((char *)">"))
  {
    ESPSerial.print(getStr);
    Serial.print("send ==> getStr: ");
    Serial.println(getStr);
    delay(500);//이 지연없이 다음 명령에서 사용 중으로 표시됩니다.
    String messageBody = "";
    while (ESPSerial.available()) 
    {
      String line = ESPSerial.readStringUntil('\n');
      if (line.length() == 1) 
      { //actual content starts after empty line (that has length 1)
        messageBody = ESPSerial.readStringUntil('\n');
      }
    }
    Serial.print("MessageBody received: ");
    Serial.println(messageBody);
    return messageBody;
  }
  else
  {
    ESPSerial.println("AT+CIPCLOSE");     // alert user
    Serial.println("ESP8266 CIPSEND ERROR: RESENDING"); //Resend...
    spare = spare + 1;
    error=1;
    return "error";
  } 
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
