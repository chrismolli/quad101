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

void serialEvent(){
  if (Serial.available() > 0){
    char firstInput = (char)Serial.read();
    //while(Serial.available()) Serial.read();
    switch (firstInput) {

      //Roll Constants
      case 114: // compares firstInput to 'r' for roll constants
        rotors.stop();
        rotors.positionController.setRollConstantsViaSerial();
        rotors.start(BEFORE_TAKE_OFF_SIGNAL);
        break;

      //Pitch Constants
      case 112: // compares firstInput to 'p' for pitch constants
      rotors.stop();
      rotors.positionController.setPitchConstantsViaSerial();
      rotors.start(BEFORE_TAKE_OFF_SIGNAL);
      break;

      //Height Control constants
      case 104: //compares firstInput to 'h'
        rotors.stop();
        rotors.heightController.setConstantsViaSerial();
        rotors.start(BEFORE_TAKE_OFF_SIGNAL);
        break;

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

      //landing the copter using heightController
      case 108: //compares firstInput to 'l'
        rotors.heightController.targetHeight = REFERENCEHEIGHT;
        break;

      default:
        Serial.println("Your first Input could not be recognized. Try again");
        //while(Serial.available()) Serial.read();
        break;
    }

    Serial.println("What would you like to update?");
    Serial.println("controller: 'c'");
    Serial.println("height: 'h'");
    Serial.println("angle: 'a'");
  }
}
