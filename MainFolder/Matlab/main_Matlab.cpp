/*
Script for advanced IMU testing.
Using complementary und kalman filtering for more precise results on the
gyroscope input of the Arduino 101.
*/

#include "Arduino.h"
#include <Timer.h>
#include "../lib/sensorfuse/sensorfuse.h"
#include "../lib/Control/PIDControl.h"
#include "../lib/Control/RotorControl.h"
#include "../lib/Control/HeightControl.h"
#include "../params.h"

//declare Timer object
Timer t;
IMU imu;
PIDControl pidController;
RotorControl rotors;

//declare necessary Variabels
float targetPosition[3] = {0,0,0}; //reference_angle
float targetHeight;
int charInput; //== 'R'

void timerUpdate(){
  imu.update(SAMPLE_RATE);
  pidController.update(rotors.RotorSignal, imu.rot, imu.rot_vel, targetPosition, SAMPLE_RATE);
  rotors.update();
}

void setup(){
  //Start Serial
  Serial.begin(38400);
  while (!Serial);
   //calibrate sensors
  imu.begin();
  pidController.begin(); //calibrate PID_Regler to forget integrated sum (I)
  rotors.begin(); //set Rotors and ESCs to PINs and initialize
  rotors.start();
  //Set timer event, that calls updateIMU every SAMPLE_RATE milliseconds
  t.every(SAMPLE_RATE,timerUpdate);
  Serial.end();
  //---------------------------------------//
  //start Serial communication with Matlab
  Serial.begin(38400);
  /*while(!Serial);
  //handshake between Arduino and Matlab
  Serial.println('a');
  char a = 'b';
  while (a != 'a'){
    a = Serial.read();
  }*/
//-----------------------------------------//
}

void loop(){
  t.update();
}

void serialEvent(){
  if (Serial.available() > 0){
    charInput = Serial.read();
    if (charInput == 'R'){ // char 52
      Serial.println(imu.rot[0]);
    }
    /*if (charInput == 'V'){
      Serial.println(imu.rot_vel[0]);
    }*/
  }
}
