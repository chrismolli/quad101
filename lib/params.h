#ifndef PARAMS_H
#define PARAMS_H

/*-----------------------------------------------------------------------*/
  //Global constants
  #define SAMPLE_RATE 5     //update Frequency for sensors and Rotors

/*-----------------------------------------------------------------------*/
  //PID constants
  #define MAX_E_FOR_I 45    //Maximum angle to contribute to I_Controller
  #define MIN_E_FOR_I 0.3   //Minimum angle to start I_controller

  #define K_P_START   0.006         //Global gain on PIDControl
  #define T_I_START   400         //T_N
  #define T_D_START   100          //T_V
  #define T_DD_START  20          //only used in PIDD2Control.h
  /*--------------------------------------*/
  /*PID control setups

    Setup         K_P       T_I       T_D   T_DD    SampleRate  TakeOff  Rotor    ESCPIN1     ESCPIN2

    Frame M2      0.012     0         5000          5           1150     Green    6           5
    Testbed M2    0.00042   2400      500           5           1150     Green    3           5
    Testbed M2    0.000165  10000     710           3           1450     Green    3           5
    Testbed M2    0.000200  10000     820           5           1450     Green    3           5
    Testbed M2    0.0005    2400      400           5           1150     Red      3           5
    Testbed M2    0.00045   2400      550           5           1300     Red      3           5
    Testbed M2    0.0004    2400      680           5           1300     Red      3           5
    Testbed M2    0.001     2000      330   370     5           1250     Red      3           5
    Testbed M2    0.006     400       100   20      5           1250     Red      3           5
    -->für SR = 200HZ ist so 0.007/8 das Maximum für Kp. Gutes Verhältnis für T_D/T_DD = 5
    -->Verhältnis optimieren und eventuell mit Matlab simulieren versuchen
  */
  /*--------------------------------------*/

/*-----------------------------------------------------------------------*/
  //Rotorcontrol constants
  //define Output Pins for ESCs
  #define ESCPIN1 3 //für Chris hier ne 6
  #define ESCPIN2 5
  #define ESCPIN3 6
  #define ESCPIN4 9

  //RotorSignal limits (from 1012 until 2012 you have increase in F)
  #define MAX_ROTOR_SIGNAL 1500
  #define MIN_ROTOR_SIGNAL 1015
  #define TAKE_OFF_SIGNAL  1250

/*-----------------------------------------------------------------------*/
  //Sensor constants
  #define COMPLEMENTARY_WEIGHT 0.98
  #define GYRORANGE 2
  #define MAG_PLUGGED_IN FALSE
  #define DECLINATION_ANGLE_DEGREE 2.1664

#endif
