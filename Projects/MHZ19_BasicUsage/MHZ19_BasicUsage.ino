#include <Arduino.h>
#include "MHZ19.h"                                        
#include <SoftwareSerial.h>                                // Remove if using HardwareSerial

#define RX_PIN 10                                          // Rx pin which the MHZ19 Tx pin is attached to
#define TX_PIN 11                                          // Tx pin which the MHZ19 Rx pin is attached to
#define PWM_PIN 14
#define ANALOGPIN A1          
#define BAUDRATE 9600                                      // Device to MH-Z19 Serial baudrate (should not be changed)

MHZ19 myMHZ19;                                             // Constructor for library
SoftwareSerial mySerial(RX_PIN, TX_PIN);                   // (Uno example) create device to MH-Z19 serial

unsigned long getDataTimer = 0;

void setup()
{
    pinMode(PWM_PIN, INPUT);
    digitalWrite(PWM_PIN, HIGH);
    pinMode(ANALOGPIN, INPUT_PULLUP);     
    Serial.begin(9600);                                     // Device to serial monitor feedback
    mySerial.begin(BAUDRATE);                               // (Uno example) device to MH-Z19 serial start   
    myMHZ19.begin(mySerial);                                // *Serial(Stream) refence must be passed to library begin(). 

    myMHZ19.autoCalibration(false);                              // Turn auto calibration ON (OFF autoCalibration(false))

  char myVersion[4];          
  myMHZ19.getVersion(myVersion);

  Serial.print("\nFirmware Version: ");
  for(byte i = 0; i < 4; i++)
  {
    Serial.print(myVersion[i]);
    if(i == 1)
      Serial.print(".");    
  }
   Serial.println("");

   Serial.print("Range: ");
   Serial.println(myMHZ19.getRange());   
   Serial.print("Background CO2: ");
   Serial.println(myMHZ19.getBackgroundCO2());
   Serial.print("Temperature Cal: ");
   Serial.println(myMHZ19.getTempAdjustment());
   Serial.print("ABC Status: "); myMHZ19.getABC() ? Serial.println("ON") :  Serial.println("OFF");
}

void loop()
{
    if (millis() - getDataTimer >= 2000)
    {
        int CO2; 

        /* note: getCO2() default is command "CO2 Unlimited". This returns the correct CO2 reading even 
        if below background CO2 levels or above range (useful to validate sensor). You can use the 
        usual documented command with getCO2(false) */


        
        CO2 = myMHZ19.getCO2();                             // Request CO2 (as ppm)
        
        Serial.print("CO2 (ppm): ");  Serial.print(CO2);  Serial.print("\t");                           

        int8_t Temp;
        Temp = myMHZ19.getTemperature();                     // Request Temperature (as Celsius)
        Serial.print("Temperature (C): ");  Serial.print(Temp); Serial.print("\t");    

        unsigned int CO2pwm;
        CO2pwm = getPpmPWM(PWM_PIN); 
        Serial.print("CO2 (ppm PWM): ");  Serial.print(CO2pwm);  Serial.print("\t");    

        float adjustedADC = analogRead(ANALOGPIN);   
        Serial.print("Analog Read Pin: "); Serial.print((int)adjustedADC); Serial.print("\t");
        //adjustedADC = 6.4995*adjustedADC - 590.53; // format; y=mx+c
        //my calculation (0,4v~2.0:0ppm-2000ppm)
        adjustedADC = 6.1035*adjustedADC - 500;
        Serial.print("Analog CO2: ");   Serial.println(adjustedADC); 
        getDataTimer = millis();
    }
}
