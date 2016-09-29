/*
Script for evaluating computing times
*/

#include "Arduino.h"
#include <Timer.h>
#include "../lib/sensorfuse/sensorfuse.h"
#include "../lib/control/PIDControl.h"
#include "../lib/control/RotorControl.h"
#include "../lib/control/HeightControl.h"
#include "../lib/params.h"

//declare necessary objects
Timer t;
IMU imu;
PIDControl pidController;
RotorControl rotors;

//declare necessary Variabels
float t1, t2, t3, t4;
float targetPosition[3] = {0,0,0};

void timerUpdate(){
  t1 = micros();
  imu.update(SAMPLE_RATE);
  t2 = micros();
  pidController.update(rotors.RotorSignal, imu.rot, imu.rot_vel, targetPosition, SAMPLE_RATE);
  t3 = micros();
  rotors.update();
  t4 = micros();
  Serial.print("IMU: ");
  Serial.print(t2-t1);
  Serial.print(" PIDController: ");
  Serial.print(t3-t2);
  Serial.print(" Rotors: ");
  Serial.print(t4-t3);
  Serial.print(" Total: ");
  Serial.println(t4-t1);
}

void setup(){
  //Start Serial and wait for connection
  Serial.begin(38400);
  while(!Serial);

   //calibrate sensors
  imu.begin();
  pidController.begin(); //calibrate PID_Regler to forget integrated sum (I)
  rotors.begin(); //set Rotors and ESCs to PINs and initialize
  rotors.start(TAKE_OFF_SIGNAL);

  //Set timer event, that calls updateIMU every SAMPLE_RATE milliseconds
  t.every(SAMPLE_RATE,timerUpdate);
}

void loop(){
  t.update();
}
