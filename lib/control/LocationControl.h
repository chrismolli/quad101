#ifndef LOCATIONCONTROL_H
#define LOCATIONCONTROL_H

/*
  Class to control the location of the multicopter.
  It uses gps coordinates to determine the distance to the targetLocation
*/

/*==================================================================*/
  //Extern librarys
  #include "../params.h"
  #include "Arduino.h"
  #include "../sensors/GPS/SKM53.h"

/*==================================================================*/
  //Classdefinition
  class LOCATIONCONTROL{
    private:

    public:
      float targetLocation[2];

      void begin(void);
      void update(float RotorSignal[4], float Position[2], float looptime);
  };

  /*==================================================================*/
    //Private Functions




/*==================================================================*/
  //Public Functions
  void LOCATIONCONTROL::begin(void){

  }
void LOCATIONCONTROL::update(float RotorSignal[4], float Position[2], float looptime){


}

#endif
