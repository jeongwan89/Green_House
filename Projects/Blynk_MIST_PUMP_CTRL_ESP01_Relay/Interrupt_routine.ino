int EMS_count=0;
unsigned long EMS_last = millis();
void EMS(){
  if(millis()-EMS_last >= 500){
    EMS_count = 0;
    EMS_last = millis();
  }
  if(EMS_count < 3){
    EMS_count += 1;
    Serial.print("Count is under"); Serial.println(EMS_count);
    return;
  }
  //MIST_STATE를 초기화
  MIST_STATE = false;
  //GrnNm를 초기화
  GrnNm = 0;
  digitalWrite(Motor, LOW);
  for(int i=0; i<4; i++) {
    digitalWrite(MIST[i], LOW);
  }
  WC_STATE = false;
  WCNm = 0;
  for(int i=0; i<3; i++){
    digitalWrite(WC[i], LOW);
  }
  EMS_count = 0;
  delay(100);
  /* 이하 코드를 실행시키면,  setup()이 실행되지 않아서 주석처리함
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Interrupt Pressed");*/
  Serial.println("Interrupt Button Pressed");
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int NEXT_count=0;
unsigned long NEXT_last = millis();
void MIST_NEXT(){
  if(millis()-NEXT_last >= 500){
    NEXT_count = 0;
    NEXT_last = millis();
  }
  if(NEXT_count < 3){
    NEXT_count += 1;
    Serial.print("Count(MIST) is under"); Serial.println(NEXT_count);
    return;
  }
  //MIST_STATE가 멈춰있으면 바로 끝
  if(MIST_STATE == false) return;
  //한동 관수가 끝났다고 알림
  startWateringTime -= (wateringSeconds *1000);
  NEXT_count = 0;
  delay(100);
  /* 이하 코드를 실행시키면,  setup()이 실행되지 않아서 주석처리함
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Interrupt Pressed");*/
  Serial.println("Interrupt(MIST-NEXT) Button Pressed");
}
