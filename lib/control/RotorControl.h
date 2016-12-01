#ifndef ROTORCONTROL_H
#define ROTORCONTROL_H

/*==================================================================*/
  //Extern librarys
  #include "Arduino.h"
  #include "Servo.h"
  #include "../params.h"
  #include "PositionControl.h"
  #include "HeightControl.h"
  #include "../sensors/sensors.h"
  #include "LocationControl.h"
  #include "../radiocontrol/radiocontrol.h"

/*==================================================================*/
  //Classdefinition
  class ROTORCONTROL{
  private:
    Servo esc1, esc2, esc3, esc4;
    SENSORS* sensors;

  public:
    uint8_t safetyModeOn;
    uint8_t safetyModeFlag;

    POSITIONCONTROL positionController;
    HEIGHTCONTROL heightController;
    LOCATIONCONTROL locationController;
    RADIOCONTROL radioControl;

    float RotorSignal[4];
    float RoundSignal[4];

    void begin(SENSORS* sensorPointer);
    void updatePosition(void);
    void updateHeight(void);
    void start(int startValue);
    void stop(void);
    int setRotorSignalViaSerial(void);
    void debug(void);
    void setESC1(int input);
  };

/*==================================================================*/
  //General Functions (begin, start, stop)
void ROTORCONTROL::begin(SENSORS* sensorPointer){

  if(Serial) Serial.print("Starting Rotorcontrol...");

  //Establish Connection to ESCs
  esc1.attach(ESCPIN0);
  esc2.attach(ESCPIN1);
  esc3.attach(ESCPIN2);
  esc4.attach(ESCPIN3);

  //Initialize pointer to sensors
  sensors = sensorPointer;

  //Initialize Rotors and be careful with delays!!
  esc1.writeMicroseconds(0);
  delay(100);
  esc2.writeMicroseconds(0);
  delay(100);
  esc3.writeMicroseconds(0);
  delay(100);
  esc4.writeMicroseconds(0);
  delay(100);
  esc1.writeMicroseconds(1012);
  delay(100);
  esc2.writeMicroseconds(1012);
  delay(100);
  esc3.writeMicroseconds(1012);
  delay(100);
  esc4.writeMicroseconds(1012);
  delay(100);

  //Initialize RotorSignals
  RotorSignal[0] = 0;
  RotorSignal[1] = 0;
  RotorSignal[2] = 0;
  RotorSignal[3] = 0;

  RoundSignal[0] = 0;
  RoundSignal[1] = 0;
  RoundSignal[2] = 0;
  RoundSignal[3] = 0;

  //Initialize PositionControl and HeightControl
  positionController.begin();
  if (HEIGHTCONTROL_ON) heightController.begin();
  if (LOCATIONCONTROL_ON) locationController.begin(sensors);
  if(RADIO_CONTROL_ON) radioControl.begin(&positionController);

  if(AUTOSTART) safetyModeOn = 0;
  else safetyModeOn = 1;

  safetyModeFlag = 1;
  if(Serial) Serial.println(" Done");
}

void ROTORCONTROL::start(int startValue){
  //accelerate motors to startValue
  //can be used as the first start before take Off via HeightControl
  if(Serial) Serial.print("Starting Rotors...");
  int s = MIN_ROTOR_SIGNAL;

  while (s < startValue){
    esc1.writeMicroseconds(s);
    esc2.writeMicroseconds(s);
    esc3.writeMicroseconds(s);
    esc4.writeMicroseconds(s);
    s = s + 40;
    delay(40);
  }

  esc1.writeMicroseconds(startValue);
  esc2.writeMicroseconds(startValue);
  esc3.writeMicroseconds(startValue);
  esc4.writeMicroseconds(startValue);

  //write start values to the RotorSignals
  RotorSignal[0] = startValue;
  RotorSignal[1] = startValue;
  RotorSignal[2] = startValue;
  RotorSignal[3] = startValue;

  if(Serial) Serial.println(" Done");
}

void ROTORCONTROL::stop(void){
  //stop procedure testbed
  int s = STARTUP_SIGNAL;
  while (s > MIN_ROTOR_SIGNAL){
    esc1.writeMicroseconds(s);
    esc2.writeMicroseconds(s);
    esc3.writeMicroseconds(s);
    esc4.writeMicroseconds(s);
    s = s - 40;
    delay(20);
  }

  esc1.writeMicroseconds(1012);
  esc2.writeMicroseconds(1012);
  esc3.writeMicroseconds(1012);
  esc4.writeMicroseconds(1012);

  //reset rotorsignals
  RotorSignal[0] = 0;
  RotorSignal[1] = 0;
  RotorSignal[2] = 0;
  RotorSignal[3] = 0;
}

/*==================================================================*/
  //Update Functions
void ROTORCONTROL::updatePosition(void){
  //Check if radiostart is on
  if(RADIO_CONTROL_ON){
    if(radioControl.ch5>1400) safetyModeOn=0;
    else safetyModeOn=1;
  }

  if ( !safetyModeOn && !safetyModeFlag ){
    //Activate RC
    if(RADIO_CONTROL_ON) radioControl.update(RotorSignal);

    //update PositionControl
    positionController.update(RotorSignal, sensors->imu.rot, sensors->imu.rot_vel, SAMPLE_RATE);

    //rotorSignal mustn't exceed limits
    //Rotor 0
    if (RotorSignal[0] > MAX_ROTOR_SIGNAL){ //Maximum Value
      RotorSignal[0] = MAX_ROTOR_SIGNAL;
      //Serial.println("R0 > MAX\n");
    }
    if (RotorSignal[0] < MIN_ROTOR_SIGNAL){ //Minimum Value
      RotorSignal[0] = MIN_ROTOR_SIGNAL;
      //Serial.println("R0 < MIN\n");
    }

    //Rotor 1
    if (RotorSignal[1] > MAX_ROTOR_SIGNAL){
      RotorSignal[1] = MAX_ROTOR_SIGNAL;
      //Serial.println("R1 > MAX\n");
    }
    if (RotorSignal[1] < MIN_ROTOR_SIGNAL){
      RotorSignal[1] = MIN_ROTOR_SIGNAL;
      //Serial.println("R1 < MIN\n");
    }

    //Rotor 2
    if (RotorSignal[2] > MAX_ROTOR_SIGNAL){
      RotorSignal[2] = MAX_ROTOR_SIGNAL;
      //Serial.println("R1 > MAX\n");
    }
    if (RotorSignal[2] < MIN_ROTOR_SIGNAL){
      RotorSignal[2] = MIN_ROTOR_SIGNAL;
      //Serial.println("R1 < MIN\n");
    }

    //Rotor 3
    if (RotorSignal[3] > MAX_ROTOR_SIGNAL){
      RotorSignal[3] = MAX_ROTOR_SIGNAL;
      //Serial.println("R1 > MAX\n");
    }
    if (RotorSignal[3] < MIN_ROTOR_SIGNAL){
      RotorSignal[3] = MIN_ROTOR_SIGNAL;
      //Serial.println("R1 < MIN\n");
    }

    //write RotorSignal to ESCs
      RoundSignal[0] = roundf(RotorSignal[0]);
      RoundSignal[1] = roundf(RotorSignal[1]);
      RoundSignal[2] = roundf(RotorSignal[2]);
      RoundSignal[3] = roundf(RotorSignal[3]);

      esc1.writeMicroseconds((int)RoundSignal[0]);
      esc2.writeMicroseconds((int)RoundSignal[1]);
      esc3.writeMicroseconds((int)RoundSignal[2]);
      esc4.writeMicroseconds((int)RoundSignal[3]);
  }

  //stop Rotors in case of signal loss or emergency
  else if( safetyModeOn && !safetyModeFlag ){
    ROTORCONTROL::stop();
    safetyModeFlag = 1;
  }

  //start Rotors again as soon as user changes safetyModeOn to 0
  else if( !safetyModeOn && safetyModeFlag ){
    ROTORCONTROL::start(STARTUP_SIGNAL);
    safetyModeFlag = 0;
  }

}

  void ROTORCONTROL::updateHeight(void){
    if (safetyModeOn == 0){
      //check which sensor needs to be used, then start height control
      if (USR_PLUGGED_IN && sensors->in_USR_Range == 1){
        heightController.update(RotorSignal, sensors->usr.height, SLOW_SAMPLE_RATE);
      }

      if (BMP_PLUGGED_IN && sensors->in_USR_Range == 0){
        heightController.update(RotorSignal, sensors->bmp.altitude, SLOW_SAMPLE_RATE);
      }

      //RotorSignal mustn't exceed limits
      //Rotor 0
      if (RotorSignal[0] > MAX_ROTOR_SIGNAL){ //Maximum Value
        RotorSignal[0] = MAX_ROTOR_SIGNAL;
        //if (Serial) Serial.println("R0 > MAX\n");
      }
      if (RotorSignal[0] < MIN_ROTOR_SIGNAL){ //Minimum Value
        RotorSignal[0] = MIN_ROTOR_SIGNAL;
        //if (Serial) Serial.println("R0 < MIN\n");
      }

      //Rotor 1
      if (RotorSignal[1] > MAX_ROTOR_SIGNAL){
        RotorSignal[1] = MAX_ROTOR_SIGNAL;
        //if (Serial) Serial.println("R1 > MAX\n");
      }
      if (RotorSignal[1] < MIN_ROTOR_SIGNAL){
        RotorSignal[1] = MIN_ROTOR_SIGNAL;
        //if (Serial) Serial.println("R1 < MIN\n");
      }

      //Rotor 2
      if (RotorSignal[2] > MAX_ROTOR_SIGNAL){
        RotorSignal[2] = MAX_ROTOR_SIGNAL;
        //if (Serial) Serial.println("R1 > MAX\n");
      }
      if (RotorSignal[2] < MIN_ROTOR_SIGNAL){
        RotorSignal[2] = MIN_ROTOR_SIGNAL;
        //if (Serial) Serial.println("R1 < MIN\n");
      }

      //Rotor 3
      if (RotorSignal[3] > MAX_ROTOR_SIGNAL){
        RotorSignal[3] = MAX_ROTOR_SIGNAL;
        //if (Serial) Serial.println("R1 > MAX\n");
      }
      if (RotorSignal[3] < MIN_ROTOR_SIGNAL){
        RotorSignal[3] = MIN_ROTOR_SIGNAL;
        //if (Serial) Serial.println("R1 < MIN\n");
      }

    //write RotorSignal to ESCs
      RoundSignal[0] = roundf(RotorSignal[0]);
      RoundSignal[1] = roundf(RotorSignal[1]);
      RoundSignal[2] = roundf(RotorSignal[2]);
      RoundSignal[3] = roundf(RotorSignal[3]);

      esc1.writeMicroseconds((int)RoundSignal[0]);
      esc2.writeMicroseconds((int)RoundSignal[1]);
      esc3.writeMicroseconds((int)RoundSignal[2]);
      esc4.writeMicroseconds((int)RoundSignal[3]);
    }
  }

/*==================================================================*/
  //Debug Functions
void ROTORCONTROL::debug(void){
    //Prints the time and RotorSignals to serial
    Serial.println("Time: ");
    Serial.print(millis());
    Serial.print(" R0: ");
    Serial.print(RotorSignal[0]);
    Serial.print(" R1: ");
    Serial.print(RotorSignal[1]);
    Serial.print(" R2: ");
    Serial.print(RotorSignal[2]);
    Serial.print(" R3: ");
    Serial.print(RotorSignal[3]);
  }

#endif
