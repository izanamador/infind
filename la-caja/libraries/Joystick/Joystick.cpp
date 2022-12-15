#include "Joystick.h"

/* Libreria base para usar funciones de arduino */
#include "Arduino.h"

/* Libreria convertidor A/D ADS1015 */
#include <Adafruit_ADS1X15.h>


/* Constructor */
Joystick::Joystick(){
  /* Si el puerto serie está disponible está modo debug imprimir valores */
  /* Serial.println() */
  /* this->Calibration(x,0); */
  /* this->Calibration(y,0); */
}

/* Destructor */
Joystick::~Joystick()
{
  ;
}

void Joystick::Calibration(short analog_value, char channel){
  for (int i = 0; i <= MAX_VALUE; i++) {
     if(analog_value-(offsets[channel]+i) == 0){
      offsets[channel] = offsets[channel] + i;
     }else if(analog_value-(offsets[channel]-i) == 0){
      offsets[channel] = offsets[channel] - i;
     }
    }
}
