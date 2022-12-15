#include "Joystick.h"

/* Libreria base para usar funciones de arduino */
#include "Arduino.h"

/* Libreria convertidor A/D ADS1015 */
#include <Adafruit_ADS1X15.h>


/* Constructor */
Joystick::Joystick(char x_channel, char y_channel){
  coordenadas.x.channel = x_channel;
  coordenadas.y.channel = y_channel;
}

/* Destructor */
Joystick::~Joystick()
{
  ;
}

void Joystick::Setup(){
  coordenadas.x.value = ads.readADC_SingleEnded(coordenadas.x.channel);
  coordenadas.y.value = ads.readADC_SingleEnded(coordenadas.y.channel);
  this->Calibration();
}

short Joystick::GetX(){
  return coordenadas.x.value;
};

short Joystick::GetY(){
  return coordenadas.y.value;
};


void Joystick::Calibration(){
  for (int i = 0; i <= MAX_VALUE; i++) {
     if(coordenadas.x.value-(coordenadas.x.offset+i) == 0){
      coordenadas.x.offset = coordenadas.x.offset + i;
     }else if(coordenadas.x.value-(coordenadas.x.offset-i) == 0){
      coordenadas.x.offset = coordenadas.x.offset - i;
     }
     if(coordenadas.y.value-(coordenadas.y.offset+i) == 0){
      coordenadas.y.offset = coordenadas.y.offset + i;
     }else if(coordenadas.y.value-(coordenadas.y.offset-i) == 0){
      coordenadas.y.offset = coordenadas.y.offset - i;
     }
    }
}
