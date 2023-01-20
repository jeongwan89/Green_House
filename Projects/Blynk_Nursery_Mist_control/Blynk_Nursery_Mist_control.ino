/*************************************************************

  This sketch shows how to write values to Virtual Pins

  NOTE:
  BlynkTimer provides SimpleTimer functionality:
    http://playground.arduino.cc/Code/SimpleTimer

  App project setup:
    Value Display widget attached to Virtual Pin V5
 *************************************************************/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID           "TMPLxxxxxx"
#define BLYNK_DEVICE_NAME           "Device"
#define BLYNK_AUTH_TOKEN            "RBhnn6AlbpUTz0168U_ar6Di2FeeM7hl"

// Test envirement
//#define MYSSID    "KT_GiGA_08C8"
//#define MYPASS    "cf05zc0562"
//#define MYTIME    100

// Real implication
#define MYSSID    "FarmMain5G"
#define MYPASS    "wweerrtt"
#define MYTIME    1000

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = MYSSID;
char pass[] = MYPASS;

//BlynkTimer timer;

#define LED_PIN 1
#define MOTOR_PIN 0

int v0value;        //virtual Pin V0의 값
int v1value;        //virtual pin V1의 값
int v0buttonState;  //virtual pin V0 버튼이 눌러졌는지
int v1buttonState;  //virtual pin V1 버튼이 눌러졌는지

int motorState;
int motorStateV1;

unsigned long lastTime = millis();
unsigned long elapsedTime;
unsigned long duration = 15 * MYTIME;
unsigned long periodic = 15 * 60 * MYTIME;


int MotorOn(){
  digitalWrite(MOTOR_PIN, LOW);
  motorState = HIGH;
  //debug
  //  Serial.print("Motor State : "); Serial.print(motorState);
  lastTime = millis();
  //debug
  //  Serial.print("\t\tAct on time : "); Serial.println(lastTime);
  return motorState;
}

int MotorOff(){
  digitalWrite(MOTOR_PIN, HIGH);
  motorState = LOW;
  //debug
  //  Serial.print("Motor State : "); Serial.print(motorState);
  lastTime = millis();
  //debug
  //  Serial.print("\t\tAct off time: "); Serial.println(lastTime);
  return motorState;
}

BLYNK_CONNECTED() {
  Blynk.syncAll();
}

BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  if (value == 1){
    MotorOn();
  } else {
    MotorOff();
  }
  
  v0value = value;
  v0buttonState = value;
  lastTime = millis();
}

BLYNK_WRITE(V1)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  if (value == 1){
    MotorOn();
  } else {
    MotorOff();
  }
  v1value = value;
  v1buttonState = value;
  lastTime = millis();
}

BLYNK_WRITE(V2){
  duration = param.asLong();
  duration *= MYTIME;  //이 변수는 단위가 second
}

BLYNK_WRITE(V3){
  periodic = param.asLong();
  periodic *= (60*MYTIME);  //이 변수는 단위가 minute
}
/*
// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
}
*/

void setup()
{
  // Debug console
  Serial.begin(115200);
  pinMode(MOTOR_PIN, OUTPUT);
  
  Blynk.begin(auth, ssid, pass,"blynk-cloud.com", 8080);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  //timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  unsigned long current;
  
  Blynk.run();
  //timer.run(); // Initiates BlynkTimer

  current = millis();
  if(v0buttonState == 1){
    if(v0value == 1){
      if(motorState == HIGH) {
        if (current-lastTime > duration){
          //debug
          //  Serial.print("\t current : "); Serial.println(current);
          //  Serial.print("\t lastTime: "); Serial.println(lastTime);
          v0value = MotorOff();
        }
      }
    }
    else {
      if (current-lastTime > periodic-duration) {
        //debug
        //   Serial.print("\t current : "); Serial.println(current);
        //   Serial.print("\t lastTime: "); Serial.println(lastTime);
        v0value = MotorOn();
      }
    }
  }

  if(v1buttonState == 1){
    if(v1value == 1){
      if(motorStateV1 == HIGH) {
        if (current-lastTime > duration){
          v1value = MotorOff();
        }
      } 
    } 
    else {
      if (current-lastTime > periodic-duration) {
        v1value = MotorOn();
      }
    }
  }
}
