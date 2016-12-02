/*
Main for Testing accelerometer
*/

/*-----------------------------------------------------------------------*/
  //Extern libraries
  #include "Arduino.h"
  #include <Timer.h>
  #include "../lib/Sensors/sensors.h"
  #include "../lib/params.h"

/*-----------------------------------------------------------------------*/
  //Declare necessary objects
  Timer t;
  SENSORS sensors;

/*-----------------------------------------------------------------------*/
  //Declare necessary Variabels


/*-----------------------------------------------------------------------*/
  //Functions
  void timerUpdate(){
    sensors.update();
  }

  void slowTimerUpdate(){
    //sensors.adxl335.debug();
    sensors.imu.debug();
    Serial.println();
  }

void setup() {
  Serial.begin(38400);
  while (!Serial);

  Serial.println("Initializing IMU...");
  sensors.begin(NULL);
  //sensors.imu.com.calibrate();

  t.every(SAMPLE_RATE, timerUpdate);
  t.every(100, slowTimerUpdate);
  Serial.println("Initialization complete.");

  /*Serial.println(sensors.adxl335.scale);
  Serial.print("z_Offset: ");
  Serial.println(sensors.adxl335.zero_Gz);
  Serial.println("X, Y, Z");*/
}

void loop() {
  t.update();
}
