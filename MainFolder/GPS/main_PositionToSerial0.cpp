/*
Main for Sending GPS Data to Serial Port (Mac)
*/

/*-----------------------------------------------------------------------*/
  //Extern libraries
  #include "Arduino.h"
  #include <Timer.h>
  #include "../lib/sensors/sensors.h"
  #include "../lib/params.h"
  #include "../lib/sensors/GPS/SKM53.h"
  #include "SoftwareSerial.h"

/*-----------------------------------------------------------------------*/
  //Declare necessary objects
  Timer t;
  SoftwareSerial skm53Serial(RX_PIN, TX_PIN); //RX 4 geht zu TX im GPS Modul; TX 3 geht zu RX im GPS Modul
  SKM53 gpsModule;

/*-----------------------------------------------------------------------*/
  //Declare necessary Variabels


/*-----------------------------------------------------------------------*/
  //Functions
  void timerUpdate(){
    gpsModule.update();
    //Serial.println("update completed");
    //Serial.println(gpsModule.returnTotalRMCString());
    Serial.println(gpsModule.formattedGPSOutput());
    //Serial.println(gpsModule.formattedLatLng());
  }

void setup() {
  Serial.begin(38400);
  while (!Serial);
  Serial.println("Initializing GPS module...");
  gpsModule.begin(&skm53Serial);
  t.every(25, timerUpdate);
  Serial.println("Initialization complete.");
  Serial.println("Latitude, Longitude, Speed, Course");
}

void loop() {
  t.update();
}
