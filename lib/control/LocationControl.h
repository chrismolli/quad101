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
  #include "../sensors/FILTER/LocationFilter.h"

/*==================================================================*/
  //Classdefinition
  class LOCATIONCONTROL{
    private:
      LOCATIONFILTER* locationFilter;
    public:
      float atan3(float lon, float lat);
      float targetLocation[2];  //[lat, lon]
      float distance[3];        //[lat_dis, lon_dis, linear_dis]

      void begin();
      void update(float targetPosition[3]);
  };

  /*==================================================================*/
    //Private Functions
float LOCATIONCONTROL::atan3(float lon, float lat){
  //function calculates atan() so that we always get a positive angle to the north axis
  float jaw;

  //rechts oben und Norden
  if (lat>0 && lon>=0){
      jaw = atanf(lon/lat);
  }

  //links oben
  if (lat>0 && lon<0){
      jaw = atanf(lat/-lon)+3/2*PI;
  }

  //rechts unten und Süden
  if (lat<0 && lon >= 0){
      jaw = atanf(lon/lat)+PI;
  }

  //links unten
  if (lat<0 && lon<0){
      jaw = atanf(lon/lat)+PI;
  }

  //Osten
  if (lat==0 && lon>0){
      jaw = PI/2;
  }

  //Westen
  if (lat==0 && lon<0){
      jaw = 3*PI/2;
  }

  //keine Positionsänderung
  if (lat==0 && lon==0){
      jaw = 0;
  }

  return jaw*180/PI;
}

/*==================================================================*/
  //Public Functions
void LOCATIONCONTROL::begin(void){
  targetLocation[0] = locationFilter->location[0];
  targetLocation[1] = locationFilter->location[1];
  }

void LOCATIONCONTROL::update(float targetPosition[3]){
  distance[0] = (targetLocation[0]-locationFilter->location[0])*PI/180*RADIUSEQUATOR;
  distance[1] = (targetLocation[1]-locationFilter->location[1])*PI/180*RADIUSEQUATOR*cos(locationFilter->location[0]*PI/180);
  distance[2] = sqrt(pow(distance[0],2)+pow(distance[1],2));

  targetPosition[2] = LOCATIONCONTROL::atan3(distance[1], distance[0]);



}

#endif
