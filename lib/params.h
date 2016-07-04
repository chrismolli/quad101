#ifndef PARAMS_H
#define PARAMS_H

/*-----------------------------------------------------------------------*/
  //Global constants
  #define SAMPLE_RATE 5     //update Frequency for sensors and Rotors
  //#define SAMPLE_RATE1 4000 //update Frequency for new controller value

/*-----------------------------------------------------------------------*/
  //PID constants
  #define MAX_E_FOR_I 45    //Maximum angle to contribute to I_Controller
  #define MIN_E_FOR_I 0.5   //Minimum angle to start controller
  #define K_GLOBAL    3     //Global gain on PIDControl
  #define K_P_START   0.2
  #define K_I_START   0.001
  #define K_D_START   0.04

/*-----------------------------------------------------------------------*/
  //Rotorcontrol constants
  #define PWMOUT1 6         //define Output Pins for ESCs
  #define PWMOUT2 5
  #define MAX_ROTOR_SIGNAL 1300 //156 max and until 145  you haveincrease in F
  #define MIN_ROTOR_SIGNAL 1015
  #define TAKE_OFF_SIGNAL  1150

/*-----------------------------------------------------------------------*/
  //Sensor constants
  #define COMPLEMENTARY_WEIGHT 0.98
  #define GYRORANGE 2
  #define MAG_PLUGGED_IN FALSE
  #define DECLINATION_ANGLE_DEGREE 2.1664

#endif
