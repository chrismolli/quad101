/*
Main for Testing and Changing PID Control during Runtime
*/

/*==================================================================*/
  //Extern libraries
  #include "Arduino.h"
  #include <Timer.h>
  #include "../lib/sensors/sensors.h"
  #include "../lib/control/RotorControl.h"
  #include "../lib/params.h"
  #include "../lib/ble_com/ble_com.h"
  #include "SoftwareSerial.h"

/*==================================================================*/
  //Declare needed objects
  Timer t;
  SENSORS sensors;
  ROTORCONTROL rotors;
  BLE_COM ble;
  SoftwareSerial skm53Serial(RX_PIN, TX_PIN); //RX 4 geht zu TX im GPS Modul; TX 2 geht zu RX im GPS Modul

/*==================================================================*/
  //Functions
void timerUpdate(){
  sensors.update();
  rotors.update();
}

void slowTimerUpdate(){
  sensors.updateSlow();
  rotors.updateSlow();
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
  if(AUTOSTART) rotors.start(BEFORE_TAKE_OFF_SIGNAL);

  //Activate untethered communication
  if(BLE_TELEMETRICS_ON) ble.begin(&rotors);

  //Set timer event, that calls timerUpdate every SAMPLE_RATE milliseconds
  t.every(SAMPLE_RATE,timerUpdate);
  t.every(SLOW_SAMPLE_RATE,slowTimerUpdate);
}

void loop(){
  t.update();
}
