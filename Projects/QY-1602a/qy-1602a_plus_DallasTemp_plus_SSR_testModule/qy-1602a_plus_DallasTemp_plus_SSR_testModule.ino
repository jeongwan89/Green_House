// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>

// Data wire is plugged into port 6 on the Arduino
#define ONE_WIRE_BUS 6

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);


const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const int Control_SSR = 9;
const int LED = 13;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int backLight = 10;
unsigned long workingTime;
unsigned long oldmillis = millis();

void setup() {

    // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");

  // Start up the library
  sensors.begin();

  
  pinMode(backLight,OUTPUT);
  pinMode(Control_SSR,OUTPUT);
  pinMode(LED,OUTPUT);
  analogWrite(backLight, 254);
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.clear();        //clear the screen
  //lcd.setCursor(0,0); //set cursor to colume0, row 0 (the first row)
  // Print a message to the LCD.
  //  lcd.print("hello, world!");
  //  lcd.setCursor(0,1);  //move cursor down one
  //lcd.print("www.abrushfx.com");  //input your text here
}

void loop() {
  
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  Serial.print("Temperature for the device 1 (index 0) is: ");
  Serial.println(sensors.getTempCByIndex(0));  
  
  // (note: line 1 is the second row, since counting begins with 0):
  // print the number of seconds since reset:
  lcd.setCursor(0, 0);
  lcd.print(millis() / 1000);
  
  //온도 표시
  lcd.setCursor(0, 1);
  lcd.print("Temp: "); lcd.print(sensors.getTempCByIndex(0));

  //정해진 온도에 SSR에 연결된 부하 작동
  //참고로 Contr_SSR 핀이 13번이고 이 핀은 내부 LED와 연결되어 있음
  if(sensors.getTempCByIndex(0) <= 28 ) {
    digitalWrite(Control_SSR, HIGH);
    digitalWrite(LED,HIGH);
     
    lcd.setCursor(7,0);
    lcd.print("WT=");
    if(millis() >= oldmillis){ 
      workingTime += (millis()-oldmillis);
      //debugging section for workingTime
      //Serial.print(millis()); Serial.print(",");Serial.print(oldmillis);Serial.print(" ");Serial.println(workingTime);
      lcd.print(workingTime/1000);
    }
  }
  else {
    digitalWrite(Control_SSR,LOW);
    digitalWrite(LED,LOW);
  }
  oldmillis = millis();
}

