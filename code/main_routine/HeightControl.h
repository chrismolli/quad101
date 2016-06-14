#ifndef HEIGHTCONTROL_H
#define HEIGHTCONTROL_H

// constants for HeightControl
#define H_P 1 //positiv
//#define H_I 1 erstmal nur PD_Regler
#define H_D 1

float height;
float e_height; //Differenz W-Y
float U_height; //Regelgröße aufgrund Höhe
float old_e_height;

void update_Heightregulation(float height, float target_height, float RotorSignal[4], float looptime){

  e_height = target_height - height; //get difference
  U_height = H_P * e_height + H_D * (e_height-old_e_height)/(looptime/1000); //PD_Regler
  old_e_height = e_height; //assign new difference to old one for next loop

  //add or subtract to RotorSignal
  RotorSignal[0] = RotorSignal[0] + U_height;
  RotorSignal[1] = RotorSignal[1] + U_height;
  RotorSignal[2] = RotorSignal[2] + U_height;
  RotorSignal[3] = RotorSignal[3] + U_height;

}


#endif
