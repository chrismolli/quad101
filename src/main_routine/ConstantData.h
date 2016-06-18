#ifndef CONSTANTDATA_H
#define CONSTANTDATA_H

//Main function
#define COMP_WEIGHT 0.98
#define SAMPLE_RATE 10 //update Frequency for sensors and Rotors
#define SAMPLE_RATE1 4000 //update Frequency for new controller value

//PIDControl
#define MAX_E_FOR_I 45 //Maximum angle to contribute to I_Controller
#define MIN_E_FOR_ALL 0 //Minimum angle to start controller

//RotorControl
#define PWMOUT1 6 //define Output Pins for ESCs
#define PWMOUT2 3

#define MAX_ROTOR_SIGNAL 88 //156 max and until 145  you haveincrease in F
#define MIN_ROTOR_SIGNAL 48
#define TAKE_OFF_SIGNAL  68

#endif
