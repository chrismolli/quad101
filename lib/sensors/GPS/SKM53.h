#ifndef SKM53_H
#define SKM53_H

/* SKM53 is a GPS module from Skylab. Class is needed to process the NMEA-strings the module
   sends via a SoftwareSerial connection (RX/TX);
   It needs to get the Serial connection to the module from the main.
*/

/*==================================================================*/
  //Extern libraries
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "../../params.h"

/*==================================================================*/
  //Classdefinition
  class SKM53{
  private:
    //pointer to SoftwareSerial object defined in the main file
    SoftwareSerial* GPSModule;
    int stringPosition;
    int rmcSection;
    String rmc[15];
    String totalRMCString;

    void updateRMCSections(void);
    void updateLat(void);
    void updateLng(void);
    void updateSpeed(void);
    void updateCourse(void);

  public:
    float latitude;
    float longitude;
    float speedOverGround;
    float courseOverGround;

    void update(void);
    void begin(SoftwareSerial* ss);
    String returnTotalRMCString(void);
    String formattedGPSOutput(void);
  };

/*==================================================================*/
  //Public Functions
void SKM53::update(void){
  GPSModule->flush();
  while (GPSModule->available() > 0){
    GPSModule->readString();
  }
  //RMC = Recommended Minimum Specific GNSS Data
  //const char *search = "$GPRMC,\0"; funktioniert so nicht
  if (GPSModule->find("$GPRMC,")) {
    updateRMCSections();
    updateLat();
    updateLng();

  }
  else {
    Serial.println("Didn't find $GPRMC,");
  }
}

void SKM53::begin(SoftwareSerial* ss){
  //GPSModule is a pointer to the SotwareSerial which has to be defined in the main file
  GPSModule = ss;
  GPSModule->begin(SKM53_BAUDRATE);
  //Serial.println("Wating for GPS Signal");
  while (!GPSModule->available());
  //Serial.println("GPS Signal received");
}

/*=====================================*/
  //String functions
String SKM53::returnTotalRMCString(void){

  String nmeaString = "$GPRMC, ";
  for (uint i = 0; i < 15 /*sizeof(rmc)*/ ; i++){
    nmeaString += rmc[i];
    nmeaString += ", ";
  }
  return nmeaString;
}

String SKM53::formattedGPSOutput(void){
  //Return Lat and Lng ready to print to the Serial
  String returnString = "";
  char latChar[9];
  dtostrf(latitude, 4, 6, latChar); //4, 6 not sure whatfor
  for (uint i = 0; i < sizeof(latChar); i++)
  {
    returnString += latChar[i];
  }

  returnString += ", ";
  char lngChar[8]; //should be increased to 9 or even 10 if the degrees of Lng increase
  dtostrf(longitude, 4, 6, lngChar);
  for (uint i = 0; i < sizeof(lngChar); i++)
  {
    returnString += lngChar[i];
  }

  returnString += ", ";

  char speedChar[7];
  dtostrf(speedOverGround, 4, 6, speedChar);
  for (uint i = 0; i < sizeof(speedChar); i++)
  {
    returnString += speedChar[i];
  }
  returnString += ", " + rmc[7];
  return returnString;
}

/*==================================================================*/
  //Private Functions
void SKM53::updateRMCSections(void){
  totalRMCString = GPSModule->readStringUntil('\n');
  //Initializing variables to process RMCString
  stringPosition = 0;
  rmcSection = 0;
  for (uint i = 0; i < totalRMCString.length(); i++) {
    //compare String(i) == ","
    if (totalRMCString.substring(i, i + 1) == ",") {
      //if "," found, safe Substring to corresponding rmcSection
      rmc[rmcSection] = totalRMCString.substring(stringPosition, i);
      //stringPosition is always the beginning of a section and gets redefined, when a section is completed
      stringPosition = i+1;
      //rmcSection gets redefined at the same moment
      rmcSection++;
    }
    //handling the last section, which doesn't end on a ","
    if (i == totalRMCString.length() - 1) {
      //if i == "\n" substring(..., i) is correct, if not it needs to be i+1
      rmc[rmcSection] = totalRMCString.substring(stringPosition, i);
    }
  }
}

void SKM53::updateLat(void){
  //now we need to sperate the degrees from the minutes
  float latFirstDigits=0;
  float latMinutes=0;
  for (uint i = 0; i < rmc[2].length(); i++) {
    //look for the ".",because we know we have two digits before it for the Minutes
    if (rmc[2].substring(i, i + 1) == ".") {
      latFirstDigits = rmc[2].substring(0, i - 2).toFloat();
      //take the rest for the minutes
      latMinutes = rmc[2].substring(i - 2).toFloat();
    }
  }
  //check if northern or southern hemisphere
  //calculate final latitude
  if (rmc[3] == "S") {
    latitude = -latFirstDigits - latMinutes/60;
  }
  else{
    latitude = latFirstDigits + latMinutes/60;
  }
}

void SKM53::updateLng(void){
  //now we need to sperate the degrees from the minutes
  float lngFirstDigits=0;
  float lngMinutes=0;
  for (uint i = 0; i < rmc[4].length(); i++) {
    //look for the ".",because we know we have two digits before it for the Minutes
    if (rmc[4].substring(i, i + 1) == ".") {
      lngFirstDigits = rmc[4].substring(0, i - 2).toFloat();
      //take the rest for the minutes
      lngMinutes = rmc[4].substring(i - 2).toFloat();
    }
  }
  //check if eastern or western hemisphere
  //calculate final longitude
  if (rmc[5] == "W") {
    longitude = -lngFirstDigits - lngMinutes/60;
  }
  else{
    longitude = lngFirstDigits + lngMinutes/60;
  }
}

void SKM53::updateSpeed(void){
  speedOverGround = rmc[6].toFloat()*0.514444; //transform to m/s
}

void SKM53::updateCourse(void){
  courseOverGround = rmc[7].toFloat(); //in degree
}

#endif
