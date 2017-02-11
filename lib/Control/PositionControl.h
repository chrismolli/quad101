#ifndef POSITIONCONTROL_H
#define POSITIONCONTROL_H

/*
  PositionControl takes IMU measurements and calculates the control signal for
  the attitude of the Copter.
  Depending on Rotor set up you need to (un)comment the required RotorSignal
  writings and eventually include a...
  ...system matrix: In case two Rotors on the same Axis (spinning the same
   direction) have different positions to the center of mass, you need to include
   a matrix to get equal force and momentum
*/

/*==================================================================*/
  //Extern librarys
  #include "../params.h"
  #include "Arduino.h"

/*==================================================================*/
  //Classdefinition
  class POSITIONCONTROL{
    private:
      float U[3];       //controlled variable (CV)
      float iSum[3];    //controlled integral variabel
      float old_dE[3];  //used for DDController
      float e[3];       //control difference (W-Y) W = Sollwert Y = Istwert

      float PController(float e, float k);
      float IController(float e, float k, int i, float looptime);
      float DController(float dE, float k);
      float DDController(float dE, float k, int i, float looptime);

    public:
      //Roll
      float K_P_ROLL;
      float T_I_ROLL;
      float T_D_ROLL;
      float T_DD_ROLL;

      //Pitch
      float K_P_PITCH;
      float T_I_PITCH;
      float T_D_PITCH;
      float T_DD_PITCH;

      //Jaw
      float K_P_JAW;
      float T_I_JAW;
      float T_D_JAW;
      float T_DD_JAW;

      float targetPosition[3];

      void reset(void);
      void begin(void);
      void update(float RotorSignal[4], float Y[3], float dE[3], float looptime);
      void setRollConstantsViaSerial(void);
      void setPitchConstantsViaSerial(void);
  };


/*==================================================================*/
  //Private Functions
float POSITIONCONTROL::PController(float e, float k){
  return k*e;
}

float POSITIONCONTROL::IController(float e, float k, int i, float looptime){
  if ((abs(e) < MAX_E_FOR_I) && (abs(e) > MIN_E_FOR_I)) {
     iSum[i] = iSum[i] + e;
  }
  return (1/k)*iSum[i]*(looptime/1000);
}

float POSITIONCONTROL::DController(float dE, float k){
  return -k * dE; //sign always needs to inverse movement
}

float POSITIONCONTROL::DDController(float dE, float k, int i, float looptime){
  float DDValue = -k*(dE-old_dE[i])/(looptime/1000);
  old_dE[i] = dE;
  return DDValue;
}

/*==================================================================*/
  //Public Functions

void POSITIONCONTROL::reset(void){
  //initialize I_Controller
  iSum[0] = 0;
  iSum[1] = 0;
  iSum[2] = 0;

  //initialize DD_Controller
  old_dE[0] = 0;
  old_dE[1] = 0;
  old_dE[2] = 0;
  }

void POSITIONCONTROL::begin(void){

  POSITIONCONTROL::reset();

  //initialize control constants
  K_P_ROLL = K_P_ROLL_START;
  T_I_ROLL = T_I_ROLL_START;
  T_D_ROLL = T_D_ROLL_START;
  T_DD_ROLL = T_DD_ROLL_START;

  K_P_PITCH = K_P_PITCH_START;
  T_I_PITCH = T_I_PITCH_START;
  T_D_PITCH = T_D_PITCH_START;
  T_DD_PITCH = T_DD_PITCH_START;

  K_P_JAW = K_P_JAW_START;
  T_I_JAW = T_I_JAW_START;
  T_D_JAW = T_D_JAW_START;
  T_DD_JAW = T_DD_JAW_START;

  //initialize target position
  targetPosition[0]=0;
  targetPosition[1]=0;
  if (RADIO_CONTROL_ON) targetPosition[2]=TARGET_JAW_VEL;
  else targetPosition[2]=TARGET_JAW;
}

void POSITIONCONTROL::update(float RotorSignal[4], float Y[3], float dE[3], float looptime){
  //PITCH & ROLL control difference
  e[0] = targetPosition[0]-Y[0];
  e[1] = targetPosition[1]-Y[1];

  //JAW control difference
  if (!RADIO_CONTROL_ON){
    e[2] = targetPosition[2]-Y[2];                       //usual case
    if (abs(targetPosition[2]-Y[2]) > 180){              //control Difference > 180
      e[2] = 360 - abs(targetPosition[2]-Y[2]);          //control difference if target < actual value
      if (targetPosition[2] > Y[2]) e[2] = -e[2];        //reverse sign if target > actual value
    }
  }
  else e[2] = targetPosition[2]-dE[2];
/*====================================*/
  //Calculating cotrolled variable (CV)
  //Roll
  U[0] = K_P_ROLL*(PController(e[0], 1) + IController(e[0], T_I_ROLL, 0, looptime) + DController(dE[0], T_D_ROLL) + DDController(dE[0], T_DD_ROLL, 0, looptime));

  //Pitch
  U[1] = K_P_PITCH*(PController(e[1], 1) + IController(e[1], T_I_PITCH, 1, looptime) + DController(dE[1], T_D_PITCH) + DDController(dE[1], T_DD_PITCH, 1, looptime));

  //Jaw
  if (RADIO_CONTROL_ON) U[2] = K_P_JAW*(e[2]+DDController(dE[2], T_DD_JAW, 2, looptime));
  else U[2] = K_P_JAW*(PController(e[2], 1) + IController(e[2], T_I_JAW, 2, looptime) + DController(dE[2], T_D_JAW) + DDController(dE[2], T_DD_JAW, 2, looptime));


/*====================================*/
  //Multiplication with system matrix
  //U[0] = 1 * U[0];
  //U[1] = 1 * U[1];
  //U[2] = 49.271 * U[2];
/*====================================*/
  //change RotorSignals due to controlled variabels
  //remember that controlDifference = target-real
  //X positioning of sensors to rotors
  //Roll
  RotorSignal[1] -= U[0];
  RotorSignal[2] -= U[0];
  RotorSignal[0] += U[0];
  RotorSignal[3] += U[0];

  //Pitch
  RotorSignal[2] -= U[1];
  RotorSignal[3] -= U[1];
  RotorSignal[0] += U[1];
  RotorSignal[1] += U[1];

  //Jaw
  if (RotorSignal[0] >= MINIMUM_THROTTLE_FOR_POSITIONCONTROL && RotorSignal[2] >= MINIMUM_THROTTLE_FOR_POSITIONCONTROL){
    RotorSignal[0] += U[2];
    RotorSignal[2] += U[2];
    RotorSignal[1] -= U[2];
    RotorSignal[3] -= U[2];
  }
  /*
  //+ positioning
  //x_Axis
  RotorSignal[0] = RotorSignal[0] + U[0];
  RotorSignal[1] = RotorSignal[1] - U[0];
  //y_axis
  RotorSignal[2] = RotorSignal[2] + U[1];
  RotorSignal[3] = RotorSignal[3] - U[1];
  //z-axis
  RotorSignal[0] = RotorSignal[0] + U[2];
  RotorSignal[1] = RotorSignal[1] + U[2];
  RotorSignal[2] = RotorSignal[2] - U[2];
  RotorSignal[3] = RotorSignal[3] - U[2];
  */
}

#endif
