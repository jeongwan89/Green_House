void tab_setup()
{
    //Pin에 Digital output 할당
  pinMode(Motor, OUTPUT);
  pinMode(Mist1, OUTPUT);
  pinMode(Mist2, OUTPUT);
  pinMode(Mist3, OUTPUT);
  pinMode(Mist4, OUTPUT);
  pinMode(WC1, OUTPUT);
  pinMode(WC2, OUTPUT);
  pinMode(WC4, OUTPUT);
  digitalWrite(Motor, LOW);
  digitalWrite(Mist1, LOW);
  digitalWrite(Mist2, LOW);
  digitalWrite(Mist3, LOW);
  digitalWrite(Mist4, LOW);
  digitalWrite(WC1, LOW);
  digitalWrite(WC2, LOW);
  digitalWrite(WC4, LOW);
  //Pin에 Digital input할당
  pinMode(EM_srply, INPUT);
  pinMode(WC_scheduled, INPUT);
  //input pin에 internal pull-up사용하기
  digitalWrite(EM_srply, HIGH);
  digitalWrite(WC_scheduled, HIGH);
  //interrupt pin 정의
  pinMode(interrupt0, INPUT);
 
  //For Liquid Crystal Display I2C
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  //-----------------------------------------------------------
  //인터럽트 처리루틴 : 인터럽트 처리는 FALLING일 때 호출
  //EMS(emergence stop)
  attachInterrupt(0, EMS, LOW);
  attachInterrupt(1, MIST_NEXT, LOW);
  
  //For initialize Real Time Clock DS1302
    Serial.begin(9600);

    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);

    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    printDateTime(compiled);
    Serial.println();

    if (!Rtc.IsDateTimeValid()) 
    {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

        Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);
    }

    if (Rtc.GetIsWriteProtected())
    {
        Serial.println("RTC was write protected, enabling writing now");
        Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled) 
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (now == compiled) 
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }
}
