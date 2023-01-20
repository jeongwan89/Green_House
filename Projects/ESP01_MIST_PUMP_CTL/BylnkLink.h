// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID           "TMPLxxxxxx"
#define BLYNK_DEVICE_NAME           "Device"
#define BLYNK_AUTH_TOKEN            "9EEDtuuCQnA5_O8k9fyOSh4M0nCBft5f"

// Test envirement
#define MYSSID    "KT_GiGA_08C8"
#define MYPASS    "cf05zc0562"
#define MYTIME    1000

// Real implication
//#define MYSSID    "FarmMain5G"
//#define MYPASS    "wweerrtt"
//#define MYTIME    1000

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

// 아래에 있는 것 2개 중에 하나만 선택할 것
//#include <WiFiESP.h>
#include <ESP8266_Lib.h>

//#include <BlynkSimpleEsp8266.h>
#include <BlynkSimpleShieldEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = MYSSID;
char pass[] = MYPASS;

// Hardware Serial on Mega, Leonardo, Micro...
//#define EspSerial Serial1

// or Software Serial on Uno, Nano...
#include <SoftwareSerial.h>
SoftwareSerial EspSerial(16, 17); // RX, TX

#define ESP8266_BAUD 9600

ESP8266 wifi(&EspSerial);
