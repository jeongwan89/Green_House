void setup() {
//  pinMode(HARDWARE_RESET,OUTPUT); /*ESP-01 하드웨어리셋 코드*/
//  digitalWrite(HARDWARE_RESET, HIGH);

  lcd.begin(16, 2);
  lcd.createChar(0, customChar);
  lcd.createChar(1, HD);
  
  Serial.begin(115200);
// 센서 초기화부분-------------------------------------------
  ESPSerial.begin(speed8266); //ESP-01모듈과 UART 시리얼 연결
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

  //ESP-01모듈 리셋------------------------------------------------
  while (!sendATcommand("AT+RST", "ready", 3)) {
    Serial.println("**Error during reset ESP-01...**");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error during reset ESP");
  }
  //초기화가 넘어가면
    Serial.println("**ESP-01 has started...");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ESP01 has started");
  
  //소프트AP+스테이션 모드로 설정--------------------------------------
  if(!sendATcommand("AT+CWMODE=3", "OK", 3)){
    Serial.println("**Error in changing mode...");
    while(1);
  }
  else {
    Serial.println("**ESP-01 has been set to mode 3...");
  }

  //AP 접속
  if(!connectWifi()){
    Serial.println("**Error in connecting to AP...");
    while(connectWifi());
  }
  else{
    Serial.println("**ESP-01 has been connected to AP...");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("ESP01 connected to AP...");
  }


/*ESP8266접속후 Blynk 실행-------------------------------------------
//  Blynk.begin(auth, ESPSerial);
//----------------------------------------------------------------- */

  lcd.setCursor(0, 0);
  lcd.print("Air:");
  lcd.setCursor(0, 1);
  lcd.print("Soi:");
  
  startWriteTiming = millis(); // starting the "program clock"

}
