void loop(){
  
  unsigned long elapsedReadingTimeCO2;
  
  start: //label 
  error=0;
  
  elapsedWriteTime = millis()-startWriteTiming; 
  elapsedReadingTimeCO2 = millis()-lastReadTimingCO2;
  readSensors();
  
  if (elapsedWriteTime > (writeTimingSeconds*1000)) 
  {

    writeThingSpeak();
    startWriteTiming = millis();   
  }    
  if (error==1) //Resend if transmission is not completed 
  {       
    Serial.println(" <<<< ERROR >>>>");
    delay (2000);  
    goto start; //go to label "start"
  }  

  //CO2 Sensor Reading during CO2interval
  if(elapsedReadingTimeCO2 > (CO2interval*1000)){
    // CO2 센서를 더이상 멤버 함수로 쓰지 않고 외부 함수로 돌려서 쓰도록 함
    // 이유. 아날로그 핀처럼 그냥 pulseIn함수로 유저 프로그램 위에서 쓰는 것이 더 가벼움
    // CO2ppm = mhz19_pwm->getPpmPwm();
    CO2pwm = getPpmPWM(PWM_PIN); 
    lastReadTimingCO2;
  }
  while(Serial.available()){
    ESPSerial.write(Serial.read());
  }

  while(ESPSerial.available()){
    Serial.write(ESPSerial.read());
  }

//soilhumidity의 평균값 계산---------------------------------------------
  for(int i = 0; i < querryCount-1 ; i++){
    averagesoilHum[i] = averagesoilHum[i+1];
  }
  averagesoilHum[querryCount-1] = soilHum;

  for(int i = 1; i < querryCount ; i++) {
    averagesoilHum[0] += averagesoilHum[i];
  }
  averagesoilHum[0] = averagesoilHum[0]/querryCount;
//---------------------------------------------------------------------

  
  
  Serial.print("Air Temp:");  Serial.print(airTemp);
  Serial.print("\tAir Humi:");  Serial.print(airHum);
  Serial.print("\tSoil Tem:");  Serial.print(soilTemp);
  Serial.print("\tSoil Hum:");  Serial.print(soilHum); Serial.print("\t"); Serial.print(analHum);
  Serial.print("\tAvr SHum:");  Serial.print(averagesoilHum[0]);
  Serial.print("\tVPD:"); Serial.print(HD_VPD);
  Serial.print("\tLight   :");  Serial.print(light); Serial.print("\t"); Serial.print(analight);
  Serial.print("\tCO2:"); Serial.print(CO2pwm); Serial.print("\tanalog:"); Serial.print(analogRead(CO2_ANALOG_PIN)); 
  //Serial.print("\tConversion:");  Serial.print((analogRead(CO2_ANALOG_PIN)-82)*2000/328); 
  Serial.println("");

//print LCD----------------------------------------------------------
  //for airTemp
  lcd.setCursor(4, 0);
  lcd.print(airTemp);   lcd.write(byte(0));
  //for airHum
  if(airHum < 10) {lcd.setCursor(8, 0);} else {lcd.setCursor(7, 0);}
  lcd.print(airHum);    lcd.print("% ");
  //for soilTemp
  lcd.setCursor(4, 1);
  lcd.print(soilTemp);  lcd.write(byte(0));
  //for soilHum
  if(averagesoilHum[0] < 10) {lcd.setCursor(7,1); lcd.print(" "); lcd.setCursor(8,1);} else {lcd.setCursor(7, 1);}
  lcd.print(averagesoilHum[0]);   lcd.print("% ");
  //for VPD
  lcd.setCursor(11,0); lcd.write(byte(1));
  if(HD_VPD.length()>= 4) {lcd.setCursor(12,0);} else {lcd.setCursor(12,0); lcd.print(" "); lcd.setCursor(13,0);}
  lcd.print(HD_VPD);
  //for light and CO2 Alternative
  if((millis()/5000)%2 == 0){
    lcd.setCursor(10,1); lcd.print("  ");
    lcd.setCursor(12,1);
    if(light < 10) {lcd.print(" ");} lcd.print(light); lcd.setCursor(14,1); lcd.print("w%");
  } else{
    lcd.setCursor(10,1); lcd.print("       ");lcd.setCursor(10,1);
    lcd.print(CO2pwm); lcd.print("ppm");
  }
/*//Blynk operation----------------------------------------------------
//Blynk.run();
//-------------------------------------------------------------------
*/  
} //End of Loop()
