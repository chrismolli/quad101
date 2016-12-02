/*
Debuging script for tilt compensated compass
*/

#include "Arduino.h"
#include "Timer.h"
#include "../lib/Sensors/IMU/imu.h"

#define SAMPLE_RATE_X 10

Timer t;
IMU imu;

void data(){
  imu.update(SAMPLE_RATE_X);
}

void serial(){
  //imu.debug();
  //imu.com.sendSerial();
  Serial.print(imu.rot[0]);
  Serial.print(",");
  Serial.print(imu.rot[1]);
  Serial.print(",");
  Serial.print(imu.rot[2]);
  Serial.println(";");
}

//Mainroutines
void setup(){
  Serial.begin(230400);
  while(!Serial);
  imu.begin();
  imu.com.calibrate();

  t.every(SAMPLE_RATE_X,data);
  t.every(10*SAMPLE_RATE_X,serial);
}

void loop(){
  //t.update();
  //if(Serial.available()) Serial.end();
}
