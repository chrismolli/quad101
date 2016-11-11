/*
Main for Testing accelerometer
*/

/*-----------------------------------------------------------------------*/
  //Extern libraries
  #include "Arduino.h"
  #include <Timer.h>
  #include "../lib/sensors/sensors.h"
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
    sensors.adxl335.debug();
    sensors.imu.debug();
    Serial.println();
  }

void setup() {
  Serial.begin(38400);
  while (!Serial);
  Serial.println("Initializing ADXL335 and main IMU...");
  sensors.begin(NULL);

  t.every(SAMPLE_RATE, timerUpdate);
  t.every(2000, slowTimerUpdate);
  Serial.println("Initialization complete.");
  Serial.print("Scale: ");
  /*Serial.println(sensors.adxl335.scale);
  Serial.print("z_Offset: ");
  Serial.println(sensors.adxl335.zero_Gz);
  Serial.println("X, Y, Z");*/
}

void loop() {
  t.update();
}
