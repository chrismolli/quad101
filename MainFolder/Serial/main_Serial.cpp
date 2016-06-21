/*
Script for advanced IMU testing.
Using complementary und kalman filtering for more precise results on the
gyroscope input of the Arduino 101.
*/

#include "Arduino.h"
#include <Timer.h>
#include "../lib/sensorfuse/sensorfuse.h"
#include "../lib/control/PIDControl.h"
#include "../lib/control/RotorControl.h"
#include "../lib/control/HeightControl.h"
#include "../lib/params.h"

//declare Timer object
Timer t;
IMU imu;
PIDControl pidController;
RotorControl rotors;

//declare necessary Variabels
float targetPosition[3] = {0,0,0}; //reference_angle
float targetHeight;

void timerUpdate(){
  imu.update(SAMPLE_RATE);
  pidController.update(rotors.RotorSignal, imu.rot, imu.rot_vel, targetPosition, SAMPLE_RATE);
  rotors.update();
}
void updateSerial(){
}

void setup(){
  //Start Serial and wait for connection
  Serial.begin(38400);
  while(!Serial);

   //calibrate sensors
  imu.begin();
  pidController.begin(); //calibrate PID_Regler to forget integrated sum (I)
  rotors.begin(); //set Rotors and ESCs to PINs and initialize
  rotors.start();
  Serial.println("What would you like to update?");
  Serial.println("controller: 'c'");
  Serial.println("height: 'h'");
  Serial.println("angle: 'a'");

  //Set timer event, that calls updateIMU every SAMPLE_RATE milliseconds
  t.every(SAMPLE_RATE,timerUpdate);
}

void loop(){
  t.update();
}

void serialEvent(){
  if (Serial.available() > 0){
    char firstInput = (char)Serial.read();
    //while(Serial.available()) Serial.read();
    switch (firstInput) {
      case 99: // compares firstInput to 'c'
        pidController.setConstantsViaSerial();
        break;
      case 104: //compares firstInput to 'h'
        rotors.setRotorSignalViaSerial();
        break;
      case 97: //compares firstInput to 'a'
        Serial.println("Enter a 2-digit number! Maximum angle is 40°.");
        Serial.println("What angle would you like to see? ");
        while (Serial.available()<2) {} //wating for Serial to have two digits
        int angleInput;
        angleInput = Serial.parseInt();
        if(angleInput <= 40){
          targetPosition[0] = angleInput;
        }
        break;
      default:
        Serial.println("Your first Input could not be recognized. Try again");
        //while(Serial.available()) Serial.read();
        break;
    }
    Serial.println("What would you like to update?");
    Serial.println("controller: 'c'");
    Serial.println("height: 'h'");
    Serial.println("angle: 'a'");
  }
}
