#ifndef NMEAPROCESSING_H
#define NMEAPROCESSING_H

/*==================================================================*/
  //Extern libraries
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "../../params.h"

/*==================================================================*/
  //Classdefinition
  class NMEAProcessing{
  private:
    //pointer to SoftwareSerial object defined in the main file
    SoftwareSerial* GPSModule;
    int stringPosition;
    int rmcSection;
    String rmc[9];
    float latitude;
    float longitude;
    void updateRMCSections(void);
    void updateLat(void);
    void updateLng(void);
    String xyz;

  public:
    void update(void);
    float returnLat(void);
    float returnLng(void);
    void begin(SoftwareSerial* ss, int baudRate);
    String returnTotalRMCString(void);
    String formattedLatLng(void);
  };

/*==================================================================*/
  //Public Functions
void NMEAProcessing::update(void){
  while (GPSModule->available() > 0){
    GPSModule->readString();
  }
  //RMC = Recommended Minimum Specific GNSS Data
  const char *search = "$GPRMC,";
  if (GPSModule->find(*search)) {
    updateRMCSections();
    updateLat();
    updateLng();
    Serial.println("SIGNAL FOUND!!!!");
  }
  else {
    Serial.println("Didn't find $GPRMC,");
  }
}

void NMEAProcessing::begin(SoftwareSerial* ss, int baudRate){
  //GPSModule is a pointer to the SotwareSerial which has to be defined in the main file
  GPSModule = ss;
  GPSModule->begin(baudRate);
  Serial.println("Wating for GPS Signal");
  while (!GPSModule->available());
  Serial.println("GPS Signal received");
}

/*=====================================*/
  //Return functions
float NMEAProcessing::returnLat(void){
  return latitude;
}

float NMEAProcessing::returnLng(void){
  return longitude;
}

/*=====================================*/
  //String functions
String NMEAProcessing::returnTotalRMCString(void){
  String nmeaString = "$GPRMC,";
  Serial.println(xyz);
  for (uint i = 0; i < 9 /*sizeof(rmc)*/ ; i++){
    nmeaString += rmc[i];
  }
  return nmeaString;
}

String NMEAProcessing::formattedLatLng(void){
  //Return Lat and Lng ready to print to the Serial
  String returnString = "Latitude: ";
  char latChar[9];
  dtostrf(latitude, 4, 6, latChar); //4, 6 not sure whatfor
  for (uint i = 0; i < sizeof(latChar); i++)
  {
    returnString += latChar[i];
  }

  returnString += " Longitude: ";
  char lngChar[9];
  dtostrf(longitude, 4, 6, lngChar);
  for (uint i = 0; i < sizeof(lngChar); i++)
  {
    returnString += lngChar[i];
  }
  return returnString;
}

/*==================================================================*/
  //Private Functions
void NMEAProcessing::updateRMCSections(void){
  String totalRMCString = GPSModule->readStringUntil('\n');
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

void NMEAProcessing::updateLat(void){
  //now we need to sperate the degrees from the minutes
  float latFirstDigits;
  float latMinutes;
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

void NMEAProcessing::updateLng(void){
  //now we need to sperate the degrees from the minutes
  float lngFirstDigits;
  float lngMinutes;
  for (uint i = 0; i < rmc[4].length(); i++) {
    //look for the ".",because we know we have two digits before it for the Minutes
    if (rmc[4].substring(i, i + 1) == ".") {
      lngFirstDigits = rmc[4].substring(0, i - 2).toFloat();
      //take the rest for the minutes
      lngMinutes = rmc[2].substring(i - 2).toFloat();
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

#endif
