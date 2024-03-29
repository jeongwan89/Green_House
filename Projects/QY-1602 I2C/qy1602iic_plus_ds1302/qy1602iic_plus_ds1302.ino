#include <stdio.h>
#include <DS1302.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


namespace {

// Set the appropriate digital I/O pin connections. These are the pin
// assignments for the Arduino as well for as the DS1302 chip. See the DS1302
// datasheet:
//
//   http://datasheets.maximintegrated.com/en/ds/DS1302.pdf
const int kCePin   = 5;  // Chip Enable
const int kIoPin   = 6;  // Input/Output
const int kSclkPin = 7;  // Serial Clock

DS1302 rtc(kCePin, kIoPin, kSclkPin);

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

}  // namespace

void setup() {
  Serial.begin(9600);

    // initialize the LCD
  lcd.begin();

  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.print("Hello, world!");

  rtc.writeProtect(false);

  // Clear all RAM bytes, using the byte interface.
  for (int i = 0; i < DS1302::kRamSize; ++i) {
    rtc.writeRam(i, 0x00);
  }

  // Write a string using the bulk interface.
  static const char kHelloWorld[] = "hello world";
  rtc.writeRamBulk((uint8_t*)kHelloWorld, sizeof(kHelloWorld) - 1);
}

void loop() {
  char buf[32];

  // Print the RAM byte values as hex using the byte interface.
  for (int i = 0; i < DS1302::kRamSize; ++i) {
    snprintf(buf, sizeof(buf), "[%02X]", rtc.readRam(i));
    Serial.print(buf); lcd.print(buf);
  }
  Serial.println(); lcd.println();

  // Print the RAM byte values as ASCII using the bulk interface.
  uint8_t ram[DS1302::kRamSize];
  rtc.readRamBulk(ram, DS1302::kRamSize);
  Serial.println((const char*)ram); lcd.println((const char*)ram);

  delay(3000);
}


