/*
Main for Sending GPS Data to Serial Port (Mac)
*/

/*-----------------------------------------------------------------------*/
  //Extern libraries
  #include "Arduino.h"
  #include <Timer.h>
  #include "../lib/sensors/sensors.h"
  #include "../lib/params.h"
  #include "../lib/sensors/GPS/NMEAProcessing.h"
  #include "SoftwareSerial.h"

/*-----------------------------------------------------------------------*/
  //Declare necessary objects
  Timer t;
  SoftwareSerial gpsSerial(RX_PIN, TX_PIN); //RX 4 geht zu TX im GPS Modul; TX 3 geht zu RX im GPS Modul
  NMEAProcessing gpsModule;

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
  gpsModule.begin(&gpsSerial, 9600);
  t.every(1000, timerUpdate);
  Serial.println("Initialization complete.");
  Serial.println("Latitude, Longitude, Speed, Course");
}

void loop() {
  t.update();
}
