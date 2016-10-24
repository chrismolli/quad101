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

/*==================================================================*/
  //Classdefinition
  class ROTORCONTROL{
  private:
    Servo esc1, esc2, esc3, esc4;
  public:

    uint8_t safetyModeOn;
    uint8_t safetyModeFlag;

    POSITIONCONTROL positionController;
    HEIGHTCONTROL heightController;

    float RotorSignal[4];
    float RoundSignal[4];

    void begin(void);
    void update(float rot[3], float rot_vel[3]);
    void updateSlow(float rot[3], float rot_vel[3], float height);
    void start(int startValue);
    void stop(void);
    int setRotorSignalViaSerial(void);
    void sendSerial(void);
    void setESC1(int input);
  };

/*==================================================================*/
  //General Functions
void ROTORCONTROL::begin(void){
  if(Serial) Serial.print("Starting Rotorcontrol...");
  //Establish Connection to ESCs
  esc1.attach(ESCPIN1);
  esc2.attach(ESCPIN2);
  esc3.attach(ESCPIN3);
  esc4.attach(ESCPIN4);

  //Initialize Rotors and be careful with delays!!
  esc1.writeMicroseconds(0);
  esc2.writeMicroseconds(0);
  esc3.writeMicroseconds(0);
  esc4.writeMicroseconds(0);
  delay(5);
  esc1.writeMicroseconds(1012);
  esc2.writeMicroseconds(1012);
  esc3.writeMicroseconds(1012);
  esc4.writeMicroseconds(1012);
  delay(5);

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
  heightController.begin();
  safetyModeOn = 0;
  safetyModeFlag = 0;
  if(Serial) Serial.println(" Done");

}

void ROTORCONTROL::start(int startValue){
  //start procedure testbed
  //could be used as the first start before take Off via HeightControl
<<<<<<< HEAD
  if(Serial) Serial.print("Starting Rotors...");
  int s = MIN_ROTOR_SIGNAL;
=======
  if(Serial) Serial.print("Starting rotors...");
  int s = 1012;
>>>>>>> 87d7df92aaddd5e1f18c43631577711dd198d2b5
  while (s < startValue){
    esc1.writeMicroseconds(s);
    esc2.writeMicroseconds(s);
    esc3.writeMicroseconds(s);
    esc4.writeMicroseconds(s);
    s = s + 15;
    delay(200);
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
<<<<<<< HEAD
=======

>>>>>>> 87d7df92aaddd5e1f18c43631577711dd198d2b5
}

void ROTORCONTROL::stop(void){
  //stop procedure testbed
  int s = BEFORE_TAKE_OFF_SIGNAL;
  while (s > MIN_ROTOR_SIGNAL){
    esc1.writeMicroseconds(s);
    esc2.writeMicroseconds(s);
    esc3.writeMicroseconds(s);
    esc4.writeMicroseconds(s);
    s = s - 20;
    delay(40);
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
void ROTORCONTROL::update(float rot[3], float rot_vel[3]){
  if ((safetyModeOn == 0) && (safetyModeFlag = 0)){
    //update PositionControl
    positionController.update(RotorSignal, rot, rot_vel, SAMPLE_RATE);

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
  else if((safetyModeOn == 1) && (safetyModeFlag == 0)){
    ROTORCONTROL::stop();
    safetyModeFlag = 1;
  }

  //start Rotors again as soon as user changes safetyModeOn to 0
  else if((safetyModeOn == 0) && (safetyModeFlag == 1)){
    ROTORCONTROL::start(BEFORE_TAKE_OFF_SIGNAL);
    safetyModeFlag = 0;
  }

}

  void ROTORCONTROL::updateSlow(float rot[3], float rot_vel[3], float height){
    if (safetyModeOn == 0){
      //update HeightControl
      heightController.update(RotorSignal, height, SLOW_SAMPLE_RATE);
      //update PositionControl (update in slowUpdate too?)
      //positionController.update(RotorSignal, rot, rot_vel, SAMPLE_RATE);

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
  }
/*==================================================================*/
  //Serial Communication Functions
int ROTORCONTROL::setRotorSignalViaSerial(void){
  Serial.print("The maximum Signal you can send to the Rotors is: ");
  Serial.println(MAX_ROTOR_SIGNAL);
  Serial.println("Always use 4 digits --> Example: 1500");
  Serial.print("Tell me which Signal you want to send to both Rotors: ");

  //wating for Serial to have four digits
  while (Serial.available()<4) {}

  int signalInput = Serial.parseInt();
  Serial.println(signalInput);
  return signalInput;
  /*if(signalInput <= MAX_ROTOR_SIGNAL){
    RotorSignal[0] = signalInput;
    RotorSignal[1] = signalInput;*/

/*if you want the regulation to be steady
  you would need to safe the original RotorSignal and compare them
  then you could add the difference!
  }*/
}

void ROTORCONTROL::sendSerial(void){
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

void ROTORCONTROL::setESC1(int input){
  esc1.writeMicroseconds(input);
}

#endif
