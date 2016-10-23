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
  #include "../lib/ble_telemetrics/ble_telemetrics.h"
  #include "SoftwareSerial.h"

/*==================================================================*/
  //Declare needed objects
  Timer t;
  SENSORS sensors;
  ROTORCONTROL rotors;
  BLE ble;
  SoftwareSerial skm53Serial(RX_PIN, TX_PIN); //RX 4 geht zu TX im GPS Modul; TX 3 geht zu RX im GPS Modul

  //needed variables
  int charInput;
  float x;

/*==================================================================*/
  //Functions
void timerUpdate(){
  sensors.update();
  rotors.update(sensors.imu.rot, sensors.imu.rot_vel);
}

void slowTimerUpdate(){
  sensors.updateSlow();
  rotors.updateSlow(sensors.imu.rot, sensors.imu.rot_vel, sensors.usr.height);
  #if BLE_TELEMETRICS_ON
    ble.update();
  #endif
}

void setup(){
  //Start Serial and wait for connection
  Serial.begin(38400);
  while(!Serial);

  //start and calibrate sensors
  sensors.begin(&skm53Serial);

  //set Rotors/ESCs to PINs and initialize
  rotors.begin();
  rotors.start(BEFORE_TAKE_OFF_SIGNAL);

  //Activate untethered communication
  #if BLE_TELEMETRICS_ON
    ble.begin(rotors,sensors);
  #endif

  //Set timer event, that calls timerUpdate every SAMPLE_RATE milliseconds
  t.every(SAMPLE_RATE,timerUpdate);
  t.every(SLOW_SAMPLE_RATE,slowTimerUpdate);
}

void loop(){
  t.update();
}

void serialEvent(){
  if (Serial.available() > 0){
    charInput = Serial.read();

    switch (charInput){
      //step Response
      //warum funktiniert step Response so nur noch bis 10 Grad??
      case 'a':
        if (rotors.positionController.targetPosition[0] == 0){
          rotors.positionController.targetPosition[0] = 20;
          Serial.println(20);
        }
        else {
          rotors.positionController.targetPosition[0] = 0;
          Serial.println(0);
        }
        break;

      //send Deflection
      case 'D':
        Serial.println(sensors.imu.rot[0]);
        break;

      //send RotorSignal
      case 'S':
        Serial.println(rotors.RotorSignal[1]);
        break;

      //send K_P
      case 'W':
        x = 10000*K_P_START;
        Serial.println(x);
        break;

      //send T_D
      case 'X':
        Serial.println(T_I_START);
        break;

      //send T_I
      case 'Y':
        Serial.println(T_D_START);
        break;

      //send T_DD
      case 'Z':
        Serial.println(T_DD_START);
        break;

      //send K_P for heightController
      case 'w':
        x = 10000*K_P_HEIGHT_START;
        Serial.println(x);
        break;

      //send T_D from heightController
      case 'x':
        Serial.println(T_I_HEIGHT_START);
        break;

      //send T_I from heightController
      case 'y':
        Serial.println(T_D_HEIGHT_START);
        break;

      //send height from heightController
      case 'h':
        Serial.println(sensors.usr.height);
        break;

      //quit/stop Process
      case 'Q':
        rotors.stop();
        //t.stop(1);
        while (Serial.available()<1) {}
        /*char2 = Serial.read();
        if (char2 == 'D')
        rotors.start(TAKE_OFF_SIGNAL);*/
        break;
    }
  }
}
