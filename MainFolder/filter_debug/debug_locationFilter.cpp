/*
Main for Sending GPS Data to Serial Port (Mac)
*/

/*-----------------------------------------------------------------------*/
  //Extern libraries
  #include "Arduino.h"
  #include <Timer.h>
  #include "../lib/sensors/sensors.h"
  #include "../lib/params.h"
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
    sensors.update();
  }

  void slowTimerUpdate(){
    sensors.updateSlow();
    sensors.locationFilter.debug();
  }

void setup() {
  Serial.begin(38400);
  while (!Serial);
  sensors.begin(&skm53Serial);
  t.every(SAMPLE_RATE, timerUpdate);
  t.every(SLOW_SAMPLE_RATE, slowTimerUpdate);
  Serial.println("Lat, Lon, gpsLat, gpsLon;");
}

void loop() {
  t.update();
}
