
////////////////////////////////////////////////////////////
#include <LiquidCrystal.h>

// LCD 핀 설정
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int keyIn=0;    // 선택된 버튼값
int btnAdc=0;   // 버튼 입력받은 ADC값

#define btnRight    0
#define btnUp       1
#define btnDown     2
#define btnLeft     3
#define btnSelect   4
#define btnDefault  5
#define pinBacklight  10

int backlightStates=1;  // backlight 제어, 1:on, 0:off

// button 읽기
int readButtons()
{
  btnAdc=analogRead(A0); // 버튼들은 A0에 연결되어 있다.

  if (btnAdc > 1000)
  {
    return btnDefault;
  }
  else if (btnAdc < 50)
  {
    return btnRight;
  }
  else if (btnAdc < 250)
  {
    return btnUp;
  }
  else if (btnAdc < 450)
  {
    return btnDown;
  }
  else if (btnAdc < 650)
  {
    return btnLeft;
  }
  else if (btnAdc < 850)
  {
    return btnSelect;
  }
}

void setup()
{
  lcd.begin(16, 2);
  Serial.begin(9600);

  pinMode(pinBacklight, OUTPUT);
}

void loop()
{
  lcd.setCursor(0, 0);
  lcd.print("Hello World ");
  lcd.print(millis());
  lcd.setCursor(0, 1);
  
  keyIn=readButtons();
  switch (keyIn)
  {
    case btnRight:
      lcd.print("Button Right");
      Serial.println("Button Right");
      backlightStates=!backlightStates;
    break;
    case btnLeft:
      lcd.print("Button Left");
      Serial.println("Button Left");
    break;
    case btnUp:
      lcd.print("Button Up");
      Serial.println("Button Up");
    break;
    case btnDown:
      lcd.print("Button Down");
      Serial.println("Button Down");
    break;
    case btnSelect:
      lcd.print("Button Select");
      Serial.println("Button Select");
    break;
    case btnDefault:
      lcd.print("Button Default");
      Serial.println("Button Default");
    break;
  }

  digitalWrite(pinBacklight, backlightStates);

  delay(100);
  lcd.clear();
}
////////////////////////////////////////////////////////////
