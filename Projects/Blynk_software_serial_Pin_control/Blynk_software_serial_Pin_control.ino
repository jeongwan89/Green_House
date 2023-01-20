/*************************************************************
  

  You’ll need:
   - Blynk IoT app (download from App Store or Google Play)
   - Arduino Pro Mini board
   - Decide how to connect to Blynk
     (USB, Ethernet, Wi-Fi, Bluetooth, ...)

  There is a bunch of great example sketches included to show you how to get
  started. Think of them as LEGO bricks  and combine them as you wish.
  For example, take the Ethernet Shield sketch and combine it with the
  Servo example, or choose a USB sketch and add a code from SendData
  example.
 *************************************************************/

// ESP-01 연결 선언부
#include <WiFiEsp.h>
#include <WiFiEspClient.h>


// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID           "TMPLxxxxxx"
#define BLYNK_DEVICE_NAME           "Device"
#define BLYNK_AUTH_TOKEN            "1lkb7FufCkp26h_-7wfnNhSrBlk-aYmI"
int status = WL_IDLE_STATUS;   // the Wifi radio's status

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

#include <BlynkSimpleStream.h>
#include <SoftwareSerial.h>

const char* WIFI_SSID = "KT_GiGA_08C8";
const char* WIFI_PW = "cf05zc0562";
WiFiEspClient esp8266Client;

char auth[] = BLYNK_AUTH_TOKEN;

SoftwareSerial SwSerial(10, 11); // RX, TX

void setup()
{
  // Debug console
  Serial.begin(115200);
  SwSerial.begin(9600);
  WiFi.init(&SwSerial);
  // check for the presence of the shield
    if (WiFi.status() == WL_NO_SHIELD) {
        Serial.println("WiFi shield not present");
    // don't continue
    while (true);
    }
  // attempt to connect to WiFi network
    while ( status != WL_CONNECTED) {
       Serial.print("Attempting to connect to WPA SSID: ");
       Serial.println(WIFI_SSID);
       // Connect to WPA/WPA2 network
       status = WiFi.begin(WIFI_SSID, WIFI_PW);
    }
  // you're connected now, so print out the data
    Serial.println("You're connected to the network"); 
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP()); 
    
  // Blynk will work through SoftwareSerial
  // Do not read or write this serial manually in your sketch
 
  Blynk.begin(SwSerial, auth);
}

void loop()
{
  Blynk.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}
