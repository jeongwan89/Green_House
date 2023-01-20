#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int backLight = 10;

void setup() {
  pinMode(backLight,OUTPUT);
  analogWrite(backLight, 254);
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.clear();        //clear the screen
  //lcd.setCursor(0,0); //set cursor to colume0, row 0 (the first row)
  // Print a message to the LCD.
  lcd.print("hello, world!");
  lcd.setCursor(0,1);  //move cursor down one
  //lcd.print("www.abrushfx.com");  //input your text here
}

void loop() {

  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis() / 1000);

}

