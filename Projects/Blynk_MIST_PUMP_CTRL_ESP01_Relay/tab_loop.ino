void tab_loop(){
  
  start: //label 
  error=0;

  //interrupt routin때문에 한줄 추가
  if((digitalRead(EM_srply)==HIGH) && (MIST_STATE == false)){
    lcd.setCursor(0,0); lcd.print("Motor OFF                  ");
  }
  if((digitalRead(WC_scheduled)==HIGH)&&(WC_STATE==false)){
    lcd.setCursor(0,1); lcd.print("Wtr_Crtn_Valve:"); lcd.print(WCNm);
  }
  //EM_srply핀의 입출력을 확인하고 바운싱 제거를 한 후, Motor_state를 결정한다.
  if((digitalRead(EM_srply) == LOW)&&(MIST_STATE == false)) { //미스트 1회 비상관수
    //EM_srply는 pull_up되어 있는 상태임으로 입력은 LOW로 확인된다.
    MIST_STATE = true;
    //-----------------------------------------------------------
    //loop에서 각 밸브의 유지 간격 시간을 결정하기 위해 프로그램 시간을 저장
    startWateringTime = millis(); // starting the "program clock"
    Serial.println("EM_srply pressed");    
    delay(50);
    // 수막 밸브와 겹치지 않게 하기 위해
    WC_STATE = false;
    digitalWrite(WC[WCNm],LOW);
  }
  //만약 MIST_STATE가 참이라면 이제 미스트 관수를 시작해야 한다.
  //미스트 동작시간 안이면 모터와 동마다 밸브를 연다.
  if(MIST_STATE == true){
    durationMilliSeconds = millis()-startWateringTime; //loop실행도중 보험삼아 실행시각에서 걸린 시간을 Monitoring한다.
                                                //unsigned long writeTimingSeconds = 17 
                                                // ==> Define Sample time in seconds to send data
    if(durationMilliSeconds < (wateringSeconds * 1000)) {
      digitalWrite(Motor, HIGH);
      lcd.setCursor(0,0);
      lcd.print("Motor ON");
      digitalWrite(MIST[GrnNm], HIGH);
      lcd.print(" House:");lcd.print(GrnNm+1);
    } 
    //미스트 관주가 정해진 시간에 이를때 미스트를 끄거나 다음 동으로 넘겨야 한다.
    else
      //마지막 동이 아닐 때, Motor는 그대로 두고 지금 온실 하우스 밸브는 닫고, 
      //하우스 넘버 올린다음,다음 온실하우스 밸브는 열고, startWateringTime 초기화
      if( GrnNm < 3) {
        digitalWrite(MIST[GrnNm], LOW);
        GrnNm += 1;
        digitalWrite(MIST[GrnNm], HIGH);
        lcd.setCursor(15,0); lcd.print(GrnNm+1);
        startWateringTime = millis();
      }      
      //다음 동으로 넘길 때 GrnNm가 마지막 동에 있으면 미스트를 끄고 GrnNm 동번호를 0으로 초기화한다
      else {
        digitalWrite(Motor, LOW);
        digitalWrite(MIST[GrnNm], LOW);
        lcd.setCursor(0,0); lcd.print("                 ");
        lcd.setCursor(0,0);
        lcd.print("Motor OFF");
        GrnNm = 0;  
        MIST_STATE = false;
      }
  }
  //WC_scheduled핀의 입출력을 확인하고 바운싱 제거
  //미스트 관수 중이면 WC_scheduled 핀 입력을 무시
  if((digitalRead(WC_scheduled) == LOW) && (WC_STATE == false) && (MIST_STATE == false)){
    //WC_STATE는 digitalWrite(WC_STATE, HIGH)되어서 pull up되어 있는 상태임
    WC_STATE = true;
    startWCTime = millis();   //계획 수막 시작을 위하여 system time을 startWCTime에 할당
    Serial.println("WC_scheduled pressed");
    delay(10);
  }
  //만약 WC_STATE가 참이라면 이제 동마다 수막밸브를 열고 닫는다.
  if(WC_STATE == true){
    WCDurationMilliSeconds = millis()-startWCTime;
    if(WCDurationMilliSeconds < (WCScheduledTime * 1000)){
      digitalWrite(WC[WCNm], HIGH);
      lcd.setCursor(0,1);
      lcd.print("Wtr_Crtn_Valve:"); 
      if(WCNm==2) {
        lcd.print(WCNm+2);
      } else {
        lcd.print(WCNm+1);
      }
    }
    else if(WCNm < 2){
      digitalWrite(WC[WCNm], LOW);
      WCNm += 1;
      digitalWrite(WC[WCNm], HIGH);
      startWCTime = millis();
    }
    else {
      digitalWrite(WC[WCNm], LOW);
      WCNm = 0;
      WC_STATE = false;
      lcd.setCursor(15,1); lcd.print("0");
    }
  }
  // For RCT
    RtcDateTime now = Rtc.GetDateTime();

    printDateTime(now);
    Serial.println();

    if (!now.IsValid())
    {
        // Common Causes:
        //    1) the battery on the device is low or even missing and the power line was disconnected
        Serial.println("RTC lost confidence in the DateTime!");
    }

    delay(100); // ten seconds
}
