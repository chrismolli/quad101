#ifndef SENSORS_H
#define SENSORS_H

/* Super header for collection all the sensor libs
*/

/*==================================================================*/
  //Sensor libraries
  #include "IMU/imu.h"

/*==================================================================*/
  //Class definition
  class SENSORS{
    public:
      IMU imu;
      
      void begin(void);
  };

/*==================================================================*/
  //Functions
  void SENSORS::begin(void){
    imu.begin();
  }

#endif
