/*
Main for Testing accelerometer
*/

/*-----------------------------------------------------------------------*/
  //Extern libraries
  #include "Arduino.h"
  #include <Timer.h>
  #include "../lib/Control/LocationControl.h"
  #include "../lib/params.h"

/*-----------------------------------------------------------------------*/
  //Declare necessary objects
  Timer t;
  LOCATIONCONTROL locationController;

/*-----------------------------------------------------------------------*/
  //Declare necessary Variabels
float x, y, erg, erg2;

/*-----------------------------------------------------------------------*/
  //Functions
void setup() {
  Serial.begin(38400);
  while (!Serial);

  Serial.println("Initializing location controller...");

  x = -1;
  y = 2;

  Serial.println("Initialization complete.");
}

void loop() {
  erg = locationController.atan3(y, x);
  erg2 = atan2f(y, x)*180/PI;
  Serial.print("atan3 = ");
  Serial.print(erg);
  Serial.print(" atan2f = ");
  Serial.println(erg2);
  Serial.println();
  delay(2000);
}
