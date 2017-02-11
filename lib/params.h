#ifndef PARAMS_H
#define PARAMS_H

/*
 ________  ___  ___  ________  ________    _____  ________    _____
|\   __  \|\  \|\  \|\   __  \|\   ___ \  / __  \|\   __  \  / __  \
 \ \  \|\  \ \  \\\  \ \  \|\  \ \  \_|\ \|\/_|\  \ \  \|\  \|\/_|\  \
  \ \  \\\  \ \  \\\  \ \   __  \ \  \ \\ \|/ \ \  \ \  \\\  \|/ \ \  \
   \ \  \\\  \ \  \\\  \ \  \ \  \ \  \_\\ \   \ \  \ \  \\\  \   \ \  \
    \ \_____  \ \_______\ \__\ \__\ \_______\   \ \__\ \_______\   \ \__\
     \|___| \__\|_______|\|__|\|__|\|_______|    \|__|\|_______|    \|__|
           \|__|
*/

/*-----------------------------------------------------------------------*/
  //1.Global constants
  #define SAMPLE_RATE 5         //update every ...ms sensors and rotors
  #define SLOW_SAMPLE_RATE 25   //update every ...ms bluetooth and usr
  #define AUTOSTART FALSE       //turn on to immediately arm the Copter

/*-----------------------------------------------------------------------*/
  //2.Communication
  #define BLE_TELEMETRICS_ON TRUE
  #define FORCE_SERIAL FALSE

  //Radio Receiver
  #define RADIO_CONTROL_ON TRUE
  #define RC_CPPM_PIN 2
  #define RC_START_BLOCKLENGTH 4000 //~µs
  #define RC_MIN 600
  #define RC_MAX 1600
  #define MAXIMUM_ANGLE 25
  #define MAXIMUM_JAW_SPEED 5

/*-----------------------------------------------------------------------*/
  //3.Rotor Control constants
  #define ESCPIN0 9   //TOP LEFT              9-----------6
  #define ESCPIN1 6   //TOP RIGHT                  |^|
  #define ESCPIN2 3   //BOTTOM RIGHT               |||
  #define ESCPIN3 5   //BOTTOM LEFT           5-----------3

  //RotorSignal limits (from 1012 until 2012 you have increase in thrust)
  #define MAX_ROTOR_SIGNAL 2000
  #define MIN_ROTOR_SIGNAL 1000
  #define STARTUP_SIGNAL 1000
  #define MINIMUM_THROTTLE_FOR_POSITIONCONTROL 1100

/*-----------------------------------------------------------------------*/
  //4.Position Control
  #define MAX_E_FOR_I 45    //Maximum angle to contribute to I_Controller
  #define MIN_E_FOR_I 0.3   //Minimum angle to start I_Controller

  #define K_P_ROLL_START   0.024         //Global gain
  #define T_I_ROLL_START   20           //T_N
  #define T_D_ROLL_START   6            //T_V
  #define T_DD_ROLL_START  5.1

  #define K_P_PITCH_START   0.03        //Global gain
  #define T_I_PITCH_START   20          //T_N
  #define T_D_PITCH_START   6           //T_V
  #define T_DD_PITCH_START  5.1

  #define K_P_JAW_START  2.2
  #define T_I_JAW_START  100000
  #define T_D_JAW_START  0
  #define T_DD_JAW_START 0.7

  //To initialize Jaw
  #define TARGET_JAW_VEL 0
  #define TARGET_JAW 0

  /*--------------------------------------*/
  /*PositionControl setups

    Setup         K_P_R   T_I  T_D  T_DD  K_P_P   T_I  T_D  T_DD  K_P_J   T_I  T_D  T_DD

    Testbed M2;5  0.006   400  100  20
    Testbed M2;5  0.0085  400  70   15
    für SR = 200HZ ist so 0.007/8 das Maximum für Kp. Gutes Verhältnis für T_D/T_DD = 5

    Frame_big M2  0.0085  400  100  86
    Frame_big M4  0.0085  400  155  125
    Frame_small   0.024   20   6    5.1  0.03    20   6    5.1   2.2     -    -    0.7
  */
  /*--------------------------------------*/

/*-----------------------------------------------------------------------*/
  //5.Height Control
  #define HEIGHTCONTROL_ON FALSE
  #define K_P_HEIGHT_START 0.02
  #define T_I_HEIGHT_START 24
  #define T_D_HEIGHT_START 8

  //minimum Height 4.1cm
  #define TARGETHEIGHTSTART 4 //cm, USR measurement in cm as well
  #define REFERENCEHEIGHT 4

  /*--------------------------------------*/
  /*HeightControl setups

    Setup         K_P       T_I    T_D   T_DD

    Frame M4      0.01975   24     4     0
  */
  /*--------------------------------------*/

/*-----------------------------------------------------------------------*/
  //6.Location Control
  #define LOCATIONCONTROL_ON FALSE
  #define K_P_LOCATION_START 0
  #define T_D_LOCATION_START 0

/*-----------------------------------------------------------------------*/
  //7.Sensor constants
  //Onboard IMU
  #define IMU_COMPLEMENTARY_WEIGHT 0.98
  #define GYRORANGE 2
  #define ACCELRANGE 2
  #define MAX_ROTATION 60 //to enable safety mode

  //Magnetometer
  #define MAG_PLUGGED_IN FALSE
  #define DECLINATION_ANGLE_DEGREE 2.1664

  //BMP180 pressure sensor
  #define BMP_PLUGGED_IN FALSE
  #define SEA_LEVEL_PRESSURE 101325 //Pa

  //USR
  #define USR_PLUGGED_IN FALSE
  #define USR_TRIGGER_PIN 13
  #define USR_ECHO_PIN 12

  //SKM53 Skylab GPS Sensor
  #define SKM53_PLUGGED_IN FALSE
  #define SKM53_BAUDRATE 9600
  #define RX_PIN 4 //goes to TX at sensor
  #define TX_PIN 2 //goes to RX at sensor
  #define GPS_LED_PIN 0 //attach to Pin 13? put 220 Ohm resistor before LED

  //ADXL335 (accelerometer) (Voltage goes to 3.3V)
  #define ADXL335_PLUGGED_IN FALSE
  #define GRAVITY 9.81
  #define XPIN A1
  #define YPIN A2
  #define ZPIN A3

/*-----------------------------------------------------------------------*/
  //8.Filter

  //Location Filter
  #define LOCATIONFILTER_ON FALSE
  #define LOCATION_COMPLEMENTARY_WEIGHT 0.95
  #define RADIUSEQUATOR 6378137 //m


#endif
