/*
Main for Testing and Changing PID Control during Runtime
*/

/*==================================================================*/
  //Extern libraries
  #include "Arduino.h"
  #include <Timer.h>
  #include "../lib/sensors/sensors.h"
  #include "../lib/control/RotorControl.h"
  #include "../lib/control/HeightControl.h"
  #include "../lib/params.h"
  #include "../lib/control/PIDD2ControlOneAxis.h"

/*==================================================================*/
  //Declare needed objects
  Timer t;
  SENSORS sensors;
  PIDD2Control controller;
  RotorControl rotors;

/*==================================================================*/
  //Declare necessary Variabels
  //referenceAngle
  float targetPosition[3] = {0,0,0};

/*==================================================================*/
  //Functions
void timerUpdate(){
  sensors.update();
  controller.update(rotors.RotorSignal, sensors.imu.rot, sensors.imu.rot_vel, targetPosition, SAMPLE_RATE);
  rotors.update();
}

void setup(){
  //Start Serial and wait for connection
  Serial.begin(38400);
  while(!Serial); //jetzt muss man einfach noch iwas drücken damits startet

   //calibrate sensors
  sensors.begin();

  //initialize PID_Regler and reset integrated sum (I)
  controller.begin();

  //set Rotors/ESCs to PINs and initialize
  rotors.begin();
  rotors.start(TAKE_OFF_SIGNAL);

  //Serial Communication
  Serial.println("What would you like to update?");
  Serial.println("controller: 'c'");
  Serial.println("height: 'h'");
  Serial.println("angle: 'a'");

  //Set timer event, that calls timerUpdate every SAMPLE_RATE milliseconds
  t.every(SAMPLE_RATE,timerUpdate);
}

void loop(){
  t.update();
}

void serialEvent(){
  if (Serial.available() > 0){
    char firstInput = (char)Serial.read();
    //while(Serial.available()) Serial.read();
    switch (firstInput) {

      case 99: // compares firstInput to 'c'
        rotors.stop();
        controller.setConstantsViaSerial();
        rotors.start(TAKE_OFF_SIGNAL);
        break;

      case 104: //compares firstInput to 'h'
        rotors.stop();
        rotors.start(rotors.setRotorSignalViaSerial());
        break;

      case 97: //compares firstInput to 'a'
        if (targetPosition[0] == 0){
          targetPosition[0] = 20;
          Serial.println("angle of 20 degree has been set!");
        }
        else {
          targetPosition[0] = 0;
          Serial.println("angle of 0 degree has been set!");
        }
        /*Serial.println("Enter a 2-digit number! Maximum angle is 45 degree. ");
        Serial.println("What angle would you like to see? ");
        while (Serial.available()<2) {} //wating for Serial to have two digits
        int angleInput;
        angleInput = Serial.parseInt();
        if(angleInput <= 40){
          targetPosition[0] = angleInput;
        }*/
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
