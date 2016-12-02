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

  String inputString;
  char inChar;
  int x1,x2,x3,x4;
/*==================================================================*/
  //Functions
void timerUpdate(){
  sensors.update();
  rotors.updatePosition();
}

void slowTimerUpdate(){
  sensors.updateSlow();
  if(HEIGHTCONTROL_ON) rotors.updateHeight();
  if(BLE_TELEMETRICS_ON) ble.update();
}

void setup(){
  //Start Serial and wait for connection
  Serial.begin(9600);
  if(FORCE_SERIAL) while(Serial.available() < 1);

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

void serialEvent(){
  x1 = micros();
  if (Serial.find("kproll")){
    inputString = Serial.readStringUntil('\n');
    rotors.positionController.K_P_ROLL = inputString.toFloat();
  }
  x2 = micros();
  else if (Serial.find("kppitch")) {
    inputString = Serial.readStringUntil('\n');
    rotors.positionController.K_P_PITCH = inputString.toFloat();
  }

  else if (Serial.find("kpjaw")){
    inputString = Serial.readStringUntil('\n');
    rotors.positionController.K_P_JAW = inputString.toFloat();
  }

  else if (Serial.find("tiroll")) {
    inputString = Serial.readStringUntil('\n');
    rotors.positionController.T_I_ROLL = inputString.toFloat();
  }

  else if (Serial.find("tipitch")) {
    inputString = Serial.readStringUntil('\n');
    rotors.positionController.T_I_PITCH = inputString.toFloat();
  }

  else if (Serial.find("tdroll")) {
    inputString = Serial.readStringUntil('\n');
    rotors.positionController.T_D_ROLL = inputString.toFloat();
  }

  else if (Serial.find("tdpitch")) {
    inputString = Serial.readStringUntil('\n');
    rotors.positionController.T_D_PITCH = inputString.toFloat();
  }

  else if (Serial.find("tddroll")) {
    inputString = Serial.readStringUntil('\n');
    rotors.positionController.T_DD_ROLL = inputString.toFloat();
  }

  else if (Serial.find("tddpitch")) {
    inputString = Serial.readStringUntil('\n');
    rotors.positionController.T_DD_PITCH = inputString.toFloat();
  }

  x3 = micros();

  Serial.print(rotors.positionController.K_P_ROLL);
  Serial.print(", ");
  Serial.println(rotors.positionController.K_P_PITCH);

  Serial.print(x2-x1);
  Serial.print(", ");
  Serial.println(x3-x1);
}
