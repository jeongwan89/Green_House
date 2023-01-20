/*
  Arduino Starter Kit example
  Project 5 - Servo Mood Indicator

  This sketch is written to accompany Project 5 in the Arduino Starter Kit

  Parts required:
  - servo motor
  - 10 kilohm potentiometer
  - two 100 uF electrolytic capacitors

  created 13 Sep 2012
  by Scott Fitzgerald

  http://www.arduino.cc/starterKit

  This example code is part of the public domain.
*/

// include the Servo library
#include <Servo.h>

Servo myServo;  // create a servo object

int const potPin = A0; // analog pin used to connect the potentiometer
int potVal;  // variable to read the value from the analog pin

int angle;   // variable to hold the angle for the servo motor
int pwm = 3;  // for output
int dir = 4;  // for output
int control_0 = 6; // for input
int control_1 = 7; // for input
int control_2 = 8; // for input
int control_dir = 12; // for input

int pwmValue; // pwm 값을 저장하는 변수

void setup() {
  pinMode(pwm, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(control_0, INPUT);
  pinMode(control_1, INPUT);
  pinMode(control_2, INPUT);
  pinMode(control_dir, INPUT);

  digitalWrite(control_0, HIGH);
  digitalWrite(control_1, HIGH);
  digitalWrite(control_2, HIGH);
  digitalWrite(control_dir, HIGH);
  myServo.attach(9); // attaches the servo on pin 9 to the servo object
  Serial.begin(9600); // open a serial connection to your computer
}

void loop() {
  potVal = analogRead(potPin); // read the value of the potentiometer
  // print out the value to the Serial Monitor
  Serial.print("potVal: ");
  Serial.print(potVal);

  // scale the numbers from the pot
  angle = map(potVal, 0, 1023, 0, 179);
  pwmValue = map(potVal, 0, 1023, 0, 255);

  // print out the angle for the servo motor
  Serial.print(", angle: ");
  Serial.println(angle);

  // set the servo position
  myServo.write(angle);
  analogWrite(pwm, pwmValue);

  // wait for the servo to get there
  delay(15);
}
