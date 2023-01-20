/* 
    NOTE - the order of functions is important for correct
    calibration.

    Where other CO2 sensors require an nitrogen atmosphere to "zero"
    the sensor CO2 reference, the MHZ19 "zero" (confusingly) refers to the 
    background CO2 level hardcoded into the device at 400ppm (getBackgroundCO2() 
    sends a command to the device to retrieve the value);

    The best start for your sensor is to wait till CO2 values are as close to background
    levels as possible (currently an average of ~418ppm). Usually at night time and outside 
    if possible, otherwise when the house has been unoccupied for as long as possible such.

    HOW TO USE:

    ----- Hardware Method  -----
    By pulling the zero HD low (0V) for 7 Secs as per the datasheet.   

    ----- Software Method -----
    Run this example while in an ideal environment (I.e. restarting the device). Once 
    restarted, disconnect MHZ19 from device and upload a new sketch to avoid
    recalibration.
    
    ----- Auto calibration ----
    If you are using auto calibration, the sensor will adjust its self every 24 hours
    (note here, the auto calibration algorithm uses the lowest observe CO2 value
    for that set of 24 hours as the zero - so, if your device is under an environment
    which does not fall to these levels, consider turning this off in your setup code). 
     
    If autoCalibration is set to "false", then getting the background calibration levels 
    correct at first try is essential.
*/

#include <Arduino.h>
#include "MHZ19.h" 
#include <SoftwareSerial.h>                                // Remove if using HardwareSerial or non-uno library compatable device

#define RX_PIN_01 10                                          
#define TX_PIN_01 11  
#define RX_PIN_02 8
#define TX_PIN_02 9
#define RX_PIN_03 6
#define TX_PIN_03 7
#define RX_PIN_04 4
#define TX_PIN_04 5                                        
#define BAUDRATE 9600                                      // Native to the sensor (do not change)

MHZ19 myMHZ19_01, myMHZ19_02, myMHZ19_03, myMHZ19_04;

SoftwareSerial mySerial_1(RX_PIN_01, TX_PIN_01);    // Uno example
//SoftwareSerial mySerial_2(RX_PIN_02, TX_PIN_02);
//SoftwareSerial mySerial_3(RX_PIN_03, TX_PIN_03);
//SoftwareSerial mySerial_4(RX_PIN_04, TX_PIN_04);
//HardwareSerial mySerial(1);               // ESP32 Example

unsigned long getDataTimer = 0;

void setup()
{
    Serial.begin(9600);

    mySerial_1.begin(BAUDRATE);                                    // Uno example: Begin Stream with MHZ19 baudrate
 //   mySerial_2.begin(BAUDRATE);
 //   mySerial_3.begin(BAUDRATE);
 //   mySerial_4.begin(BAUDRATE);
    //mySerial.begin(BAUDRATE, SERIAL_8N1, RX_PIN, TX_PIN);      // ESP32 Example

    myMHZ19_01.begin(mySerial_1);                                // *Important, Pass your Stream reference
 //   myMHZ19_02.begin(mySerial_2);
 //   myMHZ19_03.begin(mySerial_3);
 //   myMHZ19_04.begin(mySerial_4);

    /*            ### setRange(value)###  
       Basic:
       setRange(value) - set range to value (advise 2000 or 5000).
       setRange()      - set range to 2000.

       Advanced:
       Use verifyRange(int range) from this code at the bottom. 
    */
  
    myMHZ19_01.setRange(2000);                 
 //   myMHZ19_02.setRange(2000);
 //   myMHZ19_03.setRange(2000);
 //   myMHZ19_04.setRange(2000);
    /*            ###calibrateZero()###  
       Basic:
       calibrateZero() - request zero calibration

       Advanced:
       In Testing.
    */

    myMHZ19_01.calibrateZero();      
 //   myMHZ19_02.calibrateZero();      
 //   myMHZ19_03.calibrateZero();      
 //   myMHZ19_04.calibrateZero();      

    /*             ### setSpan(value)###  
       Basic:
       setSpan(value) - set span to value (strongly recommend 2000)
       setSpan()      - set span to 2000;

    */

    myMHZ19_01.setSpan(2000); 
  //  myMHZ19_02.setSpan(2000); 
  //  myMHZ19_03.setSpan(2000); 
  //  myMHZ19_04.setSpan(2000); 

    /*            ###autoCalibration(false)###  
       Basic:
       autoCalibration(false) - turns auto calibration OFF. (automatically sent before defined period elapses)
       autoCalibration(true)  - turns auto calibration ON.
       autoCalibration()      - turns auto calibration ON.

       Advanced:
       autoCalibration(true, 12) - turns autocalibration ON and calibration period to 12 hrs (maximum 24hrs).
    */

    myMHZ19_01.autoCalibration(true, 24);
//    myMHZ19_02.autoCalibration(true, 24);
//    myMHZ19_03.autoCalibration(true, 24);
//    myMHZ19_04.autoCalibration(true, 24);
    Serial.print("ABC Status: "); myMHZ19_01.getABC() ? Serial.println("myMHZ19_01 ON") :  Serial.println("myMHZ19_01 OFF");  
//    Serial.print("ABC Status: "); myMHZ19_02.getABC() ? Serial.println("myMHZ19_02 ON") :  Serial.println("myMHZ19_02 OFF");  
//    Serial.print("ABC Status: "); myMHZ19_03.getABC() ? Serial.println("myMHZ19_03 ON") :  Serial.println("myMHZ19_03 OFF");  
//    Serial.print("ABC Status: "); myMHZ19_04.getABC() ? Serial.println("myMHZ19_04 ON") :  Serial.println("myMHZ19_04 OFF");  
                                    
}

void loop()
{
    if (millis() - getDataTimer >= 2000)     // Check if interval has elapsed (non-blocking delay() equivilant)
    {
        int CO2_01,CO2_02, CO2_03, CO2_04;
        CO2_01 = myMHZ19_01.getCO2();        delay(500);
 //       CO2_02 = myMHZ19_02.getCO2();        delay(500);
 //       CO2_03 = myMHZ19_03.getCO2();        delay(500);
 //       CO2_04 = myMHZ19_04.getCO2();        delay(500);
        
        Serial.print("CO2_01 (ppm): ");  Serial.println(CO2_01);
 //       Serial.print("CO2_02 (ppm): ");  Serial.println(CO2_02);
 //       Serial.print("CO2_03 (ppm): ");  Serial.println(CO2_03);
 //       Serial.print("CO2_04 (ppm): ");  Serial.println(CO2_04);

        int8_t Temp_01, Temp_02, Temp_03, Temp_04;                           // Buffer for temperature
        Temp_01 = myMHZ19_01.getTemperature();      delay(500); // Request Temperature (as Celsius)
 //       Temp_02 = myMHZ19_02.getTemperature();      delay(500);
 //       Temp_03 = myMHZ19_03.getTemperature();      delay(500);
 //       Temp_04 = myMHZ19_04.getTemperature();      delay(500);
        Serial.print("Temperature_01 (C): ");  Serial.println(Temp_01);
 //       Serial.print("Temperature_02 (C): ");  Serial.println(Temp_02);
 //       Serial.print("Temperature_03 (C): ");  Serial.println(Temp_03);
 //       Serial.print("Temperature_04 (C): ");  Serial.println(Temp_04);

        getDataTimer = millis();              // Update inerval
    }
}
