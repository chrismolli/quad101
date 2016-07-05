#ifndef PARAMS_H
#define PARAMS_H

/*-----------------------------------------------------------------------*/
  //Global constants
  #define SAMPLE_RATE 5     //update Frequency for sensors and Rotors
  //#define SAMPLE_RATE1 4000 //update Frequency for new controller value

/*-----------------------------------------------------------------------*/
  //PID constants
  #define MAX_E_FOR_I 45    //Maximum angle to contribute to I_Controller
  #define MIN_E_FOR_I 0.3   //Minimum angle to start controller
  #define K_P_START   0.00042         //Global gain on PIDControl
  #define T_I_START   240000000         //T_N 0.00001
  #define T_D_START   500        //T_V 0.21

  //PID table
  /*setup         K_P       K_I       K_D       K_GLOBAL      PWMOUT1     PWMOUT2
    Frame M2      1         0         60        0.012         6           5
    Testbed M2             X         X         X             X           X


  */
/*-----------------------------------------------------------------------*/
  //Rotorcontrol constants
  #define ESCPIN1 3         //define Output Pins for ESCs
  #define ESCPIN2 5
  #define ESCPIN3 6
  #define ESCPIN4 9
  #define MAX_ROTOR_SIGNAL 1500 //156 max and until 145  you haveincrease in F
  #define MIN_ROTOR_SIGNAL 1015
  #define TAKE_OFF_SIGNAL  1150

/*-----------------------------------------------------------------------*/
  //Sensor constants
  #define COMPLEMENTARY_WEIGHT 0.98
  #define GYRORANGE 2
  #define MAG_PLUGGED_IN FALSE
  #define DECLINATION_ANGLE_DEGREE 2.1664

#endif
