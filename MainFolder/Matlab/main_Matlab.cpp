/*
Main for Plotting Control via Matlab
*/

/*-----------------------------------------------------------------------*/
  //Extern libraries
  #include "Arduino.h"
  #include <Timer.h>
  #include "../lib/sensorfuse/sensorfuse.h"
  #include "../lib/Control/PIDControl.h"
  #include "../lib/Control/RotorControl.h"
  #include "../lib/Control/HeightControl.h"
  #include "../lib/params.h"
  #include "../lib/Control/PIDD2Control.h"

/*-----------------------------------------------------------------------*/
  //Declare necessary objects
  Timer t;
  IMU imu;
  PIDD2Control controller;
  RotorControl rotors;

/*-----------------------------------------------------------------------*/
  //Declare necessary Variabels
  float targetPosition[3] = {0,0,0};
  float targetHeight;
  int charInput;
  float x;

/*-----------------------------------------------------------------------*/
  //Functions
void timerUpdate(){
  imu.update(SAMPLE_RATE);
  controller.update(rotors.RotorSignal, imu.rot, imu.rot_vel, targetPosition, SAMPLE_RATE);
  rotors.update();
}

void setup(){
  //Start Serial
  Serial.begin(38400);
  while (!Serial);

  //calibrate sensors
 imu.begin();
 //initialize PID_Regler and reset integrated sum (I)
 controller.begin();
 //set Rotors/ESCs to PINs and initialize
 rotors.begin();
 rotors.start(TAKE_OFF_SIGNAL);

  //Set timer event, that calls timerUpdate every SAMPLE_RATE milliseconds
  t.every(SAMPLE_RATE,timerUpdate);
  Serial.end();

  //start Serial communication with Matlab
  Serial.begin(38400);
  /*while(!Serial);
  //handshake between Arduino and Matlab
  Serial.println('a');
  char a = 'b';
  while (a != 'a'){
    a = Serial.read();
  }*/
}

void loop(){
  t.update();
}

void serialEvent(){
  if (Serial.available() > 0){
    charInput = Serial.read();

    switch (charInput){
      //step Response
      case 'a':
        targetPosition[0] = 20;
        Serial.println(0);
        break;
        
      //send Deflection
      case 'D':
        Serial.println(imu.rot[0]);
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

      //send TakeOff Signal
      case 'T':
        Serial.println(TAKE_OFF_SIGNAL);
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
