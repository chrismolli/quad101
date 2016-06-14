/*
Script for advanced IMU testing.
Using complementary und kalman filtering for more precise results on the
gyroscope input of the Arduino 101.
*/

#include "Arduino.h"
#include <Timer.h>
#include "../../lib/sensorfuse/sensorfuse.h"
#include "PIDControl.h"
#include "RotorControl.h"
#include "HeightControl.h"
#include "ConstantData.h"

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
  imu.startAndCalibrate();
  pidController.set(); //calibrate PID_Regler to forget integrated sum (I)
  pidController.initializeSerialConnection();
  rotors.initialize(); //set Rotors and ESCs to PINs and initialize
  rotors.start();

  //Set timer event, that calls updateIMU every SAMPLE_RATE milliseconds
  t.every(SAMPLE_RATE,timerUpdate);
}

void loop(){
  t.update();
}

void serialEvent(){
  pidController.setConstantsViaSerial();
}
