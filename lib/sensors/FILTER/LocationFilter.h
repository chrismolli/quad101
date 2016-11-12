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
    float latitude;
    float longitude;
    float speed[2]; //speed[lat, lon]
    float accel[2]; //accel[lat, lon]
    float distance[2]; //distance[lat, lon]

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

    latitude = skm53->latitude;
    longitude = skm53->longitude;

    speed[0] = 0;
    speed[1] = 0;
    accel[0] = 0;
    accel[1] = 0;
    distance[0] = 0;
    distance[1] = 0;
  }
void LOCATIONFILTER::imuUpdate(float looptime){
    //calculating acceleration in lat and lon direction
    //imu->accel[0] is x-direction or direction of travel or heading (all the same)
    //imu->accel[1] is y-direction
    accel[0] = imu->accel[0]*cos(imu->com.heading*PI/180)-imu->accel[1]*sin(imu->com.heading*PI/180);
    accel[1] = imu->accel[0]*sin(imu->com.heading*PI/180)-imu->accel[1]*cos(imu->com.heading*PI/180);

    speed[0] += accel[0]*looptime;
    speed[1] += accel[1]*looptime;

    distance[0] += speed[0]*looptime;
    distance[1] += speed[1]*looptime;
}

void LOCATIONFILTER::gpsUpdate(void){
    /*
      first: calculate new lat/lon refering to the imu measurements
      - calculate longitude first, because you need latitude in the equation, but it's probably no difference
      second: use complementary filter to include GPS signal
      third: reset distance to 0
    */
    longitude += distance[1]/(RADIUSEQUATOR*cos(latitude*PI/180))*180/PI;
    latitude += distance[0]/RADIUSEQUATOR*180/PI;

    longitude = LOCATION_COMPLEMENTARY_WEIGHT*longitude+(1-LOCATION_COMPLEMENTARY_WEIGHT)*skm53->longitude;
    latitude = LOCATION_COMPLEMENTARY_WEIGHT*latitude+(1-LOCATION_COMPLEMENTARY_WEIGHT)*skm53->latitude;

    distance[0] = 0;
    distance[1] = 0;
}

void LOCATIONFILTER::debug(void){
    if (Serial){
      //lat, lon, latgps, longps;
      Serial.print(latitude);
      Serial.print(", ");
      Serial.print(longitude);
      Serial.print(", ");
      Serial.print(skm53->latitude);
      Serial.print(", ");
      Serial.print(skm53->longitude);
      Serial.println("; ");

    }

}
/*==================================================================*/
  //Private Functions


#endif
