void setup() {
//  pinMode(HARDWARE_RESET,OUTPUT); /*ESP-01 하드웨어리셋 코드*/
//  digitalWrite(HARDWARE_RESET, HIGH);

  lcd.begin(16, 2);
  lcd.createChar(0, customChar);
  lcd.createChar(1, HD);
  
  Serial.begin(115200);
  
// 센서 초기화부분-------------------------------------------
  DS18B20.begin();
  dht.begin();
  //mhz19_pwm->setAutoCalibration(false);
  
  lcd.setCursor(0, 0);
  lcd.print("Resetting ESP01");
  lcd.setCursor(0, 1);
  lcd.print("MH-Z19 warming:");
  delay(3000);
  lcd.setCursor(7,1); //lcd.print(mhz19_pwm->getStatus()); lcd.print("        ");
  
  
//2021.8. ESP01 소켓사용으로 인하여 하드웨어 리셋 불가능. AT 커멘드로 대체 
//  EspHardwareReset(); //Reset do Modulo WiFi
  Serial.println("**Ready to start...**");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("**Ready to start...**");
  delay (1000);

//MQTT 초기화부분----------------------------------------------------
    ESPSerial.begin(speed8266); //ESP-01모듈과 UART 시리얼 연결
    init_MQTT_ESP01();
//-----------------------------------------------------------------


//LCD 머리말 표시부분
/*
  lcd.setCursor(0, 0);
  lcd.print("Air:");
  lcd.setCursor(0, 1);
  lcd.print("Bdg:");
*/ 

  startWriteTiming = millis(); // starting the "program clock"

}
