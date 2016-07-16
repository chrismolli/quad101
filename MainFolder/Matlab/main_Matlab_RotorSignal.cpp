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

/*-----------------------------------------------------------------------*/
  //Declare necessary objects
  Timer t;
  IMU imu;
  PIDControl pidController;
  RotorControl rotors;

/*-----------------------------------------------------------------------*/
  //Declare necessary Variabels
  float targetPosition[3] = {0,0,0};
  float targetHeight;
  int charInput;

/*-----------------------------------------------------------------------*/
  //Functions
void timerUpdate(){
  imu.update(SAMPLE_RATE);
  pidController.update(rotors.RotorSignal, imu.rot, imu.rot_vel, targetPosition, SAMPLE_RATE);
  rotors.update();
}

void setup(){
  //Start Serial
  Serial.begin(38400);
  while (!Serial);

  //calibrate sensors
 imu.begin();
 //initialize PID_Regler and reset integrated sum (I)
 pidController.begin();
 Serial.println(K_P_START);
 //set Rotors/ESCs to PINs and initialize
 rotors.begin();
 rotors.start(TAKE_OFF_SIGNAL);

  //Set timer event, that calls timerUpdate every SAMPLE_RATE milliseconds
  t.every(SAMPLE_RATE,timerUpdate);
  Serial.println(K_P_START);
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

    //send Deflection
    if (charInput == 'D'){
      Serial.println(imu.rot[0]);
    }

    //send RotorSignal
    if (charInput == 'S'){
      Serial.println(rotors.RotorSignal[1]);
    }

    //send K_P
    if (charInput == 'X'){
      Serial.println(4);
    }

    //send T_D
    if (charInput == 'Y'){
      Serial.println(T_D_START);
    }

    //send T_I
    if (charInput == 'Z'){
      Serial.println(T_I_START);
    }

    //quit/stop Process
    if (charInput == 'Q'){
      rotors.stop();
      //t.stop(1);
      while (Serial.available()<1) {}
      /*char2 = Serial.read();
      if (char2 == 'D')
      rotors.start(TAKE_OFF_SIGNAL);*/
    }
  }
}
