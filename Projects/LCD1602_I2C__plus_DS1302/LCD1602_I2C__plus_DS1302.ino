#include <stdio.h>

#include <ThreeWire.h>
#include <RtcDS1302.h>

#include<LiquidCrystal_I2C.h>
const int kIO = 3;    //DAT
const int kSCLK = 4;  //CLK
const int kCE = 2;    //RST
LiquidCrystal_I2C lcd(0x27, 16, 2); //LCD 클래스 초기화

ThreeWire myWire(kIO, kSCLK, kCE); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  //1602 LCD
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  //RTC
  checkRTC();

}

// the loop function runs over and over again forever
void loop() {
  //LED BLINK
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(500);                       // wait for a second

  //RTC show Serial and LCD
  showRTC();
}

void checkRTC(void)
{
  //RTC
  Serial.print("compiled: ");
  Serial.print(__DATE__); Serial.print(" ");  Serial.println(__TIME__);

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

void showRTC(void)
{
  //RTC
  RtcDateTime now = Rtc.GetDateTime();

  printDateTime(now);
  Serial.println();

  if (!now.IsValid())
  {
    // Common Causes:
    //    1) the battery on the device is low or even missing and the power line was disconnected
    Serial.println("RTC lost confidence in the DateTime!");
  }

  //LCD 시간 프린트
  lcd.setCursor(0, 0);
  char nowDate[20];
  snprintf_P(nowDate,
             20,
             PSTR("%04u/%02u/%02u"),
             now.Year(),
             now.Month(),
             now.Day());
  lcd.print(nowDate);

  
  lcd.setCursor(0, 1);
  char nowTime[20];
  snprintf_P(nowTime,
             20,
             PSTR("%02u:%02u:%02u"),
             now.Hour(),
             now.Minute(),
             now.Second() );
  lcd.print(nowTime);
}
#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
  char datestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%04u/%02u/%02u %02u:%02u:%02u"),
             dt.Year(),
             dt.Month(),
             dt.Day(),

             dt.Hour(),
             dt.Minute(),
             dt.Second() );
  Serial.print(datestring);
}
