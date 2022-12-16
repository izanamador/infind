#include "Joystick.h"

/* Libreria base para usar funciones de arduino */
#include "Arduino.h"

/* Libreria convertidor A/D ADS1015 */
#include <Adafruit_ADS1X15.h>

/* Libreria de uso general */
#include <math.h>


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

// String teststr = "rhyloo";

void Direction(int direction){
  static int actual_direction = 0;
  if(direction != actual_direction){
    actual_direction = direction;
    switch(direction) {
    case 0:
      // teststr = "Centro";
      Serial.println("Centro");
      break;
    case 1:
      // teststr = "Derecha";
      Serial.println("Derecha");
      break;
    case 2:
      // teststr = "Arriba";
      Serial.println("Arriba");
      break;
    case 3:
      // teststr = "Izquierda";
      Serial.println("Izquierda");
      break;
    case 4:
      // teststr = "Abajo";
      Serial.println("Abajo");
      break;
    }
  }
}

void Joystick::Setup(){
  begin();
  coordenadas.x.value = readADC_SingleEnded(coordenadas.x.channel);
  coordenadas.y.value = readADC_SingleEnded(coordenadas.y.channel);
  this->Calibration();
}

void Joystick::Loop(){

  const float DEG2RAD = PI / 180.0f;
  const float RAD2DEG = 180.0f / PI;

  coordenadas.x.value = readADC_SingleEnded(coordenadas.x.channel);
  coordenadas.y.value = readADC_SingleEnded(coordenadas.y.channel);

  coordenadas.x.value_fixed = coordenadas.x.value - coordenadas.x.offset;
  coordenadas.y.value_fixed = coordenadas.y.value - coordenadas.y.offset;

  if(abs(coordenadas.x.value_read-coordenadas.x.value_fixed) > 800){
    coordenadas.x.value_read = coordenadas.x.value_fixed;
  }

  if(abs(coordenadas.y.value_read-coordenadas.y.value_fixed) > 800){
    coordenadas.y.value_read = coordenadas.y.value_fixed;
  }

  if(abs(coordenadas.x.value_fixed) < 800){
    coordenadas.x.value_fixed = 0;
  }

  if(abs(coordenadas.y.value_fixed) < 800){
    coordenadas.y.value_fixed = 0;
  }

  angle =  atan2(coordenadas.y.value_read,coordenadas.x.value_read);

  if(coordenadas.x.value_fixed == 0 && coordenadas.y.value_fixed == 0){
    direction = 0;
  }else if (angle >= 0 ){
    if (angle <= 45*DEG2RAD) {
      direction = 1; //derecha
    } else if (angle >= (90+45)*DEG2RAD){
      direction = 3; //izquierda
    }else{
      direction = 2;} //arriba
  }else{
    if (angle >= -45*DEG2RAD){
      direction = 1; //derecha
    }else if (angle <= (-90-45)*DEG2RAD){
      direction = 3; //izquierda
    }else{
      direction = 4;} //abajo
  }

  Direction(direction);

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
