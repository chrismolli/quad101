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
  //needed objects
  Timer t;
  SENSORS sensors;
  ROTORCONTROL rotors;
  BLE_COM ble;
  SoftwareSerial skm53Serial(RX_PIN, TX_PIN); //RX 4 geht zu TX im GPS Modul; TX 2 geht zu RX im GPS Modul

  //needed variables
  int charInput;
  float x;

/*==================================================================*/
  //Functions
void timerUpdate(){
  sensors.update();
  rotors.updatePosition();
}

void slowTimerUpdate(){
  sensors.updateSlow();
  rotors.updateHeight();
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

void serialEvent(){
  if (Serial.available() > 0){
    char firstInput = (char)Serial.read();
    //while(Serial.available()) Serial.read();
    switch (firstInput) {
      //change Angle of Testbed
      case 97: //compares firstInput to 'a' to change angle
        if (rotors.positionController.targetPosition[0] == 0){
          rotors.positionController.targetPosition[0] = 5;
          Serial.println("angle of 5 degree has been set!");
        }
        else {
          rotors.positionController.targetPosition[0] = 0;
          Serial.println("angle of 0 degree has been set!");
        }
        break;

      //send Deflection of Roll
      case 'D':
        Serial.println(sensors.imu.rot[0]);
        break;

      //send RotorSignal of one Rotor
      case 'S':
        Serial.println(rotors.RotorSignal[1]);
        break;

      //send K_P
      case 'W':
        x = 10000*K_P_ROLL_START;
        Serial.println(x);
        break;

      //send T_D
      case 'X':
        Serial.println(T_I_ROLL_START);
        break;

      //send T_I
      case 'Y':
        Serial.println(T_D_ROLL_START);
        break;

      //send T_DD
      case 'Z':
        Serial.println(T_DD_ROLL_START);
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

      //quit/stop Process
      case 'Q':
        rotors.stop();
        //t.stop(1);
        while (Serial.available()<1) {}
        /*char2 = Serial.read();
        if (char2 == 'D')
        rotors.start(TAKE_OFF_SIGNAL);*/
        break;

      default:
        break;
    }
  }
}
