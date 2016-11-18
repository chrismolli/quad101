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
  #include "../sensors/sensors.h"

/*==================================================================*/
  //Classdefinition
  class LOCATIONCONTROL{
    private:
      SENSORS* sensors;
    public:
      float atan3(float lon, float lat);
      float targetLocation[2];  //[lat, lon]
      float distance[6];        //[lat_dis, lon_dis, linear_dis, old_dis_lat, old_dis_lon. old_dis_linear]

      float K_P;
      float T_D;
      float U[3];               //U[linear, lat, lon]

      void begin(SENSORS* sensorsPointer);
      void update(float targetPosition[3], float looptime);
  };

/*==================================================================*/
  //Public Functions
void LOCATIONCONTROL::begin(SENSORS* sensorsPointer){
  sensors = sensorsPointer;

  targetLocation[0] = sensors->locationFilter.location[0];
  targetLocation[1] = sensors->locationFilter.location[1];

  K_P = K_P_LOCATION_START;
  T_D = T_D_LOCATION_START;

  distance[3] = 0;
  distance[4] = 0;
  distance[5] = 0;
  }

void LOCATIONCONTROL::update(float targetPosition[3], float looptime){
  /*
    first: calculate the linear distance to the target Location
    long distance: adapt heading and pitch to fly forward
    short distance: cover location errors by hovering -> don't change heading, but pitch and roll
  */
  distance[0] = (targetLocation[0]-sensors->locationFilter.location[0])*PI/180*RADIUSEQUATOR;
  distance[1] = (targetLocation[1]-sensors->locationFilter.location[1])*PI/180*RADIUSEQUATOR*cos(sensors->locationFilter.location[0]*PI/180);
  distance[2] = sqrt(pow(distance[0],2)+pow(distance[1],2));

  //long distance control
  if (distance[2] > 5){
    targetPosition[2] = LOCATIONCONTROL::atan3(distance[1], distance[0]);
    U[0] = K_P*(distance[2]+T_D*(distance[2]-distance[5])/looptime*1000);
    if (abs(targetPosition[2]-sensors->imu.rot[2]) < 15){
      targetPosition[1] -= U[0]; //-,because clockwise
      if (targetPosition[1] < -20) targetPosition[1] = -20;
      if (targetPosition[1] >  20) targetPosition[1] =  20;
    }
  }

  //short distance control
  else {
    U[1] = K_P*(distance[0]+T_D*(distance[0]-distance[3])/looptime*1000);
    U[2] = K_P*(distance[1]+T_D*(distance[1]-distance[4])/looptime*1000);
    /*targetPosition[0] += k*U[1]+k+U[2];
    targetPosition[1] -= k*U[1]+k*U[2];*/
  }

  distance[3] = distance[0];
  distance[4] = distance[1];
  distance[5] = distance[2];
}

/*==================================================================*/
  //Private Functions
float LOCATIONCONTROL::atan3(float lon, float lat){
//function calculates atan() so that we always get a positive angle to the north axis 0°-360°
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

#endif
