#ifndef LOCATIONFILTER_H
#define LOCATIONFILTER_H

/* Class for Filtering
*/

/*==================================================================*/
  //Extern libraries
#include "Arduino.h"
#include "../../params.h"
#include "../GPS/SKM53.h"
#include "../IMU/imu.h"

/*==================================================================*/
  //Classdefinition
  class LOCATIONFILTER{
  private:
    //pointer to SKM53 object and IMU object
    SKM53* skm53;
    IMU* imu;

  public:
    float location[2];      //[lat, lon]
    float accel_local[2];   //accel in x and y direction of the quad
    float accel_global[2];  //accel[lat, lon]
    float speed[2];         //speed[lat, lon]
    float distance[2];      //distance[lat, lon]

    void begin(SKM53* gpsObject, IMU* imuObject);
    void imuUpdate(float looptime);
    void gpsUpdate(void);
    void debug(void);
  };

/*==================================================================*/
  //Public Functions
void LOCATIONFILTER::begin(SKM53* gpsObject, IMU* imuObject){
    skm53 = gpsObject;
    imu = imuObject;

    location[0] = skm53->location[0];
    location[1] = skm53->location[1];

    accel_local[0] = 0;
    accel_local[1] = 0;
    accel_global[0] = 0;
    accel_global[1] = 0;
    speed[0] = 0;
    speed[1] = 0;
    distance[0] = 0;
    distance[1] = 0;
  }
void LOCATIONFILTER::imuUpdate(float looptime){
    /*
      first: calculate acceleration in x and y direction taking position (pitch and roll)
      and gravity into account accel_local[x, y]
      second: calculate acceleration in lat and lon direction; accel_global[lat, lon]
      imu->accel[0] is x-direction or direction of travel or heading (all the same)
      imu->accel[1] is y-direction
      third: integration
    */
    accel_local[0] = (imu->accel[0]+GRAVITY*sin(imu->rot[1]*PI/180))*cos(imu->rot[1]*PI/180)+(imu->accel[2]-GRAVITY*cos(imu->rot[1]*PI/180))*sin(imu->rot[1]*PI/180);
    accel_local[1] = (imu->accel[1]-GRAVITY*sin(imu->rot[0]*PI/180))*cos(imu->rot[0]*PI/180)-(imu->accel[2]-GRAVITY*cos(imu->rot[0]*PI/180))*sin(imu->rot[0]*PI/180);

    accel_global[0] = accel_local[0]*cos(imu->rot[2]*PI/180)+accel_local[1]*sin(imu->rot[2]*PI/180);
    accel_global[1] = accel_local[0]*sin(imu->rot[2]*PI/180)+accel_local[1]*cos(imu->rot[2]*PI/180);

    speed[0] += accel_global[0]*looptime/1000;
    speed[1] += accel_global[1]*looptime/1000;

    distance[0] += speed[0]*looptime/1000;
    distance[1] += speed[1]*looptime/1000;
}

void LOCATIONFILTER::gpsUpdate(void){
    /*
      first: calculate new lat/lon refering to the imu measurements
      - calculate longitude first, because you need latitude in the equation, but it's probably no difference
      second: use complementary filter to include GPS signal
      third: reset distance to 0
    */
    location[1] += distance[1]/(RADIUSEQUATOR*cos(location[0]*PI/180))*180/PI;
    location[0] += distance[0]/RADIUSEQUATOR*180/PI;

    if (skm53->isValid == 1){
    location[1] = LOCATION_COMPLEMENTARY_WEIGHT*location[1]+(1-LOCATION_COMPLEMENTARY_WEIGHT)*skm53->location[1];
    location[0] = LOCATION_COMPLEMENTARY_WEIGHT*location[0]+(1-LOCATION_COMPLEMENTARY_WEIGHT)*skm53->location[0];

    speed[0] = LOCATION_COMPLEMENTARY_WEIGHT*speed[0]+(1-LOCATION_COMPLEMENTARY_WEIGHT)*skm53->speedOverGround*cos(skm53->courseOverGround*PI/180);
    speed[1] = LOCATION_COMPLEMENTARY_WEIGHT*speed[1]+(1-LOCATION_COMPLEMENTARY_WEIGHT)*skm53->speedOverGround*sin(skm53->courseOverGround*PI/180);
    }

    distance[0] = 0;
    distance[1] = 0;
}

void LOCATIONFILTER::debug(void){
    if (Serial){
      //lat, lon, latgps, longps, accelx, accely;
      Serial.print(location[0], 6);
      Serial.print(", ");
      Serial.print(location[1], 6);
      Serial.print(", ");
      Serial.print(skm53->location[0], 6);
      Serial.print(", ");
      Serial.print(skm53->location[1], 6);
      Serial.print(", ");
      Serial.print(speed[0]);
      Serial.print(", ");
      Serial.print(speed[1]);
      Serial.print(", ");
      Serial.print(skm53->speedOverGround);
      Serial.println(";");
    }
}
/*==================================================================*/
  //Private Functions


#endif
