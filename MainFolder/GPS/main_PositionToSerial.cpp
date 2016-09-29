/*
Main for Sending GPS Data to Serial Port (Mac)
*/

/*-----------------------------------------------------------------------*/
  //Extern libraries
  #include "Arduino.h"
  #include <Timer.h>
  #include "../lib/sensorfuse/sensorfuse.h"
  #include "../lib/params.h"
  #include "../lib/sensorfuse/GPS/NMEAProcessing.h"
  #include "SoftwareSerial.h"

/*-----------------------------------------------------------------------*/
  //Declare necessary objects
  Timer t;
  SoftwareSerial gpsSerial(RX_PIN, TX_PIN);
  NMEAProcessing gpsModule;

/*-----------------------------------------------------------------------*/
  //Declare necessary Variabels


/*-----------------------------------------------------------------------*/
  //Functions
  void timerUpdate(){
    Serial.println("start update");
    gpsModule.update();
    Serial.println("update completed");
    //t.update();
    Serial.println(gpsModule.returnTotalRMCString());
    Serial.println("loop completed");

    //Serial.println(gpsModule.formattedLatLng());
  }

void setup() {
  Serial.begin(38400);
  while (!Serial);
  Serial.println("Initializing GPS module...");
  gpsModule.begin(&gpsSerial, 9600);
  t.every(2000, timerUpdate);
  Serial.println("Initialization completed.");
}

void loop() {
  t.update();
}
