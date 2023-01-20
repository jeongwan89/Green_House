
// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include "DHT.h"

#define DHTPIN 2     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

// ESP8266 with 16x2 i2c LCD
// Compatible with the Arduino IDE 1.6.6
// Library https://github.com/agnunez/ESP8266-I2C-LCD1602
// Original Library https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
// Modified for ESP8266 with GPIO0-SDA GPIO2-SCL and LCD1206 display
// edit library and change Wire.begin() by Wire.begin(sda,scl) or other GPIO's used for I2C
// and access from lcd.begin(sda,scl)

#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); // Check I2C address of LCD, normally 0x27 or 0x3F
byte heart[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0}; // example sprite bitmap
byte degreeCelcius[] = {0x08, 0x14, 0x08, 0x06, 0x09, 0x08, 0x09, 0x06};
int LED_LAMP = 5;


void setup() {
  pinMode(LED_LAMP, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));

  lcd.begin(12, 13);     // In ESP8266-01, SDA=0, SCL=2
  lcd.backlight();
  lcd.createChar(0, heart);
  lcd.createChar(1, degreeCelcius);


  dht.begin();
}

void loop() {
  // Wait a few seconds between measurements.

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));




  lcd.home();                // At column=0, row=0
  lcd.print("Temp:");
  lcd.print(t, 1);
  lcd.write(byte(1));

  lcd.setCursor(0, 1);
  lcd.print("Humi:");
  lcd.print(h, 1);
  lcd.print("%");


  delay(500);


  digitalWrite(LED_LAMP, HIGH);
  lcd.setCursor(15, 0);      // At column=15, row=0
  lcd.write(byte(0));
  delay(500);


  
  digitalWrite(LED_LAMP, LOW);
  lcd.setCursor(15, 0);      // At column=15, row=0
  lcd.print(" ");            // Wipe sprite
}
