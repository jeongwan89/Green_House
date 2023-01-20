#include <stdio.h>

#include <ThreeWire.h>
#include <RtcDS1302.h>

#include<LiquidCrystal_I2C.h>
const int kIO = 3;    //DAT
const int kSCLK = 4;  //CLK
const int kCE = 5;    //RST

const int vol_1 = 6;
const int vol_5 = 7;
const int vol_10 = 8;
const int vol_50 = 9;
const int vol_100 = 10;
const int vol_500 = 11;
const int vol_minus = 12;

const int magneticPin = 13;
const int solValvePin = 14;

LiquidCrystal_I2C lcd(0x27, 16, 2); //LCD 클래스 초기화

ThreeWire myWire(kIO, kSCLK, kCE); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

/*
  Liquid flow rate sensor -DIYhacking.com Arvind Sanjeev

  Measure the liquid/water flow rate using this code.
  Connect Vcc and Gnd of sensor to arduino, and the
  signal line to arduino digital pin 2.

*/

byte statusLed    = 13;

byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 2;

// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 4.5;

volatile byte pulseCount;
volatile bool hallCount = true;

float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime;


void setup() {

  //1602 LCD
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  //RTC
  //checkRTC();

  // Set up the status LED line as an output
  pinMode(statusLed, OUTPUT);
  digitalWrite(statusLed, HIGH);  // We have an active-low LED attached

  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;

  // The Hall-effect sensor is connected to pin 2 which uses interrupt 0.
  // Configured to trigger on a FALLING state change (transition from HIGH
  // state to LOW state)
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
}

// the loop function runs over and over again forever
void loop() {
  if ((millis() - oldTime) > 1000)   // Only process counters once per second
  {
    // Disable the interrupt while calculating flow rate and sending the value to
    // the host
    detachInterrupt(sensorInterrupt);

    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;

    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    oldTime = millis();

    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;

    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;

    unsigned int frac;

    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.print("L/min");
    Serial.print("\t");       // Print tab space

    // Print the cumulative total of litres flowed since starting
    Serial.print("Output Liquid Quantity: ");
    Serial.print(totalMilliLitres);
    Serial.print("mL");
    Serial.print("\t");       // Print tab space
    Serial.print(totalMilliLitres / 1000);
    Serial.println("L");


    // Reset the pulse counter so we can start incrementing again
    pulseCount = 0;

    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  }

  
  //RTC show Serial and LCD
  //showRTC();
}
/*
Insterrupt Service Routine
 */
void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
  hallCount = !hallCount;
  digitalWrite(statusLed, hallCount);  // We have an active-low LED attached
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
