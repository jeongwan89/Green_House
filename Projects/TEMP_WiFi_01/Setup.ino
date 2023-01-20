void setup() {
  pinMode(HARDWARE_RESET,OUTPUT);
  digitalWrite(HARDWARE_RESET, HIGH); //출력핀 풀업저항을 달 필요가 없음
  
  pinMode(PWM_PIN, INPUT);
  digitalWrite(PWM_PIN, HIGH);
  
  lcd.begin(16, 2);
  lcd.createChar(0, customChar);
  lcd.createChar(1, HD);
  
  Serial.begin(9600);
// 센서 초기화부분-------------------------------------------
  ESPSerial.begin(speed8266); //ESP-01모듈과 UART 시리얼 연결
  DS18B20.begin();
  dht.begin();
  mhz19_pwm->setAutoCalibration(false);
  
  lcd.setCursor(0, 0);
  lcd.print("Resetting ESP01");
  lcd.setCursor(0, 1);
  lcd.print("MH-Z19 warming:");
  delay(3000);
  lcd.setCursor(7,1);// lcd.print(mhz19_pwm->getStatus()); lcd.print("        ");
  
  EspHardwareReset(); //Reset do Modulo WiFi
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
  lastReadTimingCO2S = millis();
}
