/*
Main for Sending GPS Data to Serial Port (Mac)
*/

/*-----------------------------------------------------------------------*/
  //Extern libraries
  #include "Arduino.h"
  #include <Timer.h>
  #include "../lib/Sensors/sensors.h"
  #include "../lib/params.h"
  #include "../lib/Sensors/GPS/SKM53.h"
  #include "SoftwareSerial.h"

/*-----------------------------------------------------------------------*/
  //Declare necessary objects
  Timer t;
  SoftwareSerial skm53Serial(RX_PIN, TX_PIN); //RX 4 geht zu TX im GPS Modul; TX 3 geht zu RX im GPS Modul
  SENSORS sensors;

/*-----------------------------------------------------------------------*/
  //Declare necessary Variabels


/*-----------------------------------------------------------------------*/
  //Functions
  void timerUpdate(){
    sensors.skm53.update();
    //Serial.println("update completed");
    //Serial.println(gpsModule.returnTotalRMCString());
    Serial.println(sensors.skm53.formattedGPSOutput());
    //Serial.println(gpsModule.formattedLatLng());
  }

void setup() {
  Serial.begin(38400);
  while (!Serial);
  Serial.println("Initializing GPS module...");
  sensors.begin(&skm53Serial);
  t.every(25, timerUpdate);
  Serial.println("Initialization complete.");
  Serial.println("Latitude, Longitude, Speed, Course");
}

void loop() {
  t.update();
}
