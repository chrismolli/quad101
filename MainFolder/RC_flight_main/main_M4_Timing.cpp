/*
Main for Testing and Changing PID Control during Runtime
*/

/*==================================================================*/
  //Extern libraries
  #include "Arduino.h"
  #include <Timer.h>
  #include "../lib/Sensors/sensors.h"
  #include "../lib/Control/RotorControl.h"
  #include "../lib/params.h"
  #include "../lib/Ble_com/ble_com.h"
  #include "SoftwareSerial.h"

/*==================================================================*/
  //Declare needed objects
  Timer t;
  SENSORS sensors;
  ROTORCONTROL rotors;
  BLE_COM ble;
  SoftwareSerial skm53Serial(RX_PIN, TX_PIN); //RX 4 geht zu TX im GPS Modul; TX 2 geht zu RX im GPS Modul
  double x1, x2, x3, x4;
/*==================================================================*/
  //Functions
void timerUpdate(){
  x1 = micros();
  sensors.update();
  x2 = micros();
  rotors.updatePosition();
  x3 = micros();
  Serial.print(x2-x1);
  Serial.print(", ");
  Serial.print(x3-x2);
  Serial.print(", ");
  Serial.print(x3-x1);
  Serial.println();
}

void slowTimerUpdate(){

  sensors.updateSlow();
  if(HEIGHTCONTROL_ON) rotors.updateHeight();
  if(BLE_TELEMETRICS_ON) ble.update();
}

void setup(){
  //Start Serial and wait for connection
  Serial.begin(38400);
  if(FORCE_SERIAL) while(!Serial);

  //start and calibrate sensors
  sensors.begin(&skm53Serial);

  //set Rotors/ESCs to PINs and initialize
  rotors.begin(&sensors);
  if(AUTOSTART) rotors.start(STARTUP_SIGNAL);

  //Activate untethered communication
  if(BLE_TELEMETRICS_ON) ble.begin(&rotors);

  //Set timer event, that calls timerUpdate every SAMPLE_RATE milliseconds
  t.every(SAMPLE_RATE,timerUpdate);
  t.every(SLOW_SAMPLE_RATE,slowTimerUpdate);
}

void loop(){
  t.update();
}
