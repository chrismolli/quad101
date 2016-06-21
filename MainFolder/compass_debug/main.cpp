/*
Debuging script for tilt compensated compass
*/

#include "Arduino.h"
#include "Timer.h"
#include "../lib/sensorfuse/sensorfuse.h"

#define SAMPLE_RATE 100

Timer t;
IMU imu;

//Subroutines
void updateData(){
  //Vereinfachte Datenupdates
  imu.update(SAMPLE_RATE);
  //imu.com.readTiltHeading(imu.rot);
  imu.com.read();
  //imu.com.readHeading();
}

void serial(){
  //imu.com.sendSerial();
  //imu.sendSerial();

  //Serial.print(imu.com.fixHeading);
  //Serial.print(":");
  //Serial.println(imu.com.heading);
  //Serial.print(":");
  //Serial.println(imu.com.heading-imu.com.fixHeading);


  Serial.print(imu.raw_Accel[0]);
  Serial.print(",");
  Serial.print(imu.raw_Accel[1]);
  Serial.print(",");
  Serial.print(imu.raw_Accel[2]);
  Serial.print(",");
  Serial.print(imu.com.rawMag[0]);
  Serial.print(",");
  Serial.print(imu.com.rawMag[1]);
  Serial.print(",");
  Serial.print(imu.com.rawMag[2]);
  Serial.println(";");
}

//Mainroutines
void setup(){
  Serial.begin(230400);
  while(!Serial);
  imu.startAndCalibrate();

  t.every(SAMPLE_RATE,updateData);
  t.every(SAMPLE_RATE,serial);
}

void loop(){
  t.update();
  if(Serial.available()) Serial.end();
}
