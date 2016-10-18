#ifndef PARAMS_H
#define PARAMS_H

/*-----------------------------------------------------------------------*/
  //Global constants
  #define SAMPLE_RATE 5     //update Frequency for sensors and Rotors
  #define SLOW_SAMPLE_RATE 25

/*-----------------------------------------------------------------------*/
  //PID constants
  #define MAX_E_FOR_I 45    //Maximum angle to contribute to I_Controller
  #define MIN_E_FOR_I 0.3   //Minimum angle to start I_controller

  #define K_P_START   0.0085         //Global gain on PIDControl
  #define T_I_START   400           //T_N
  #define T_D_START   150           //T_V
  #define T_DD_START  125            //only used in PIDD2Control.h

  #define K_P_JAW_START  0
  #define T_I_JAW_START  1000
  #define T_D_JAW_START  0
  #define T_DD_JAW_START 0

  /*--------------------------------------*/
  /*PID control setups

    Setup         K_P       T_I       T_D   T_DD    SampleRate  TakeOff  Rotor    ESCPIN1     ESCPIN2

    Testbed M2    0.006     400       100   20      5           1250     Red      3           5
    Testbed M2    0.0085    400       70    15      5/3         1250/1350 Red     3           5
    für SR = 200HZ ist so 0.007/8 das Maximum für Kp. Gutes Verhältnis für T_D/T_DD = 5

    Frame M2      0.007     400       121   103     5           1200     Red      6           5
    Frame M2      0.0085    400       100   86      5           1200     Red      6           5
    Frame M4      0.0085    400       155   125     5           1200     Red
  */
  /*--------------------------------------*/

/*-----------------------------------------------------------------------*/
  //Rotorcontrol constants
  //define Output Pins for ESCs
  #define ESCPIN1 6
  #define ESCPIN2 5
  #define ESCPIN3 3
  #define ESCPIN4 9

  //RotorSignal limits (from 1012 until 2012 you have increase in F)
  #define MAX_ROTOR_SIGNAL 2000
  #define MIN_ROTOR_SIGNAL 1015
  #define TAKE_OFF_SIGNAL  1600

/*-----------------------------------------------------------------------*/
  //Sensor constants
  #define COMPLEMENTARY_WEIGHT 0.98
  #define GYRORANGE 2

  #define MAG_PLUGGED_IN FALSE
  #define DECLINATION_ANGLE_DEGREE 2.1664

  #define BMP_PLUGGED_IN FALSE
  #define SEA_LEVEL_PRESSURE 101325 //Pa

  #define USR_PLUGGED_IN FALSE
  #define USR_TRIGGER_PIN 13
  #define USR_ECHO_PIN 12

  #define BLE_TELEMETRICS_ON FALSE

  //Skylab GPS Sensor
  #define RX_PIN 4
  #define TX_PIN 3

#endif
