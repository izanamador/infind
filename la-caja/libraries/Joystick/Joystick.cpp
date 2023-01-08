#include "Joystick.h"

/* Constructor */
Joystick::Joystick(char x_channel, char y_channel){
  coordenada.x.channel = x_channel;
  coordenada.y.channel = y_channel;
}

/* Destructor */
Joystick::~Joystick(){
  
}

void Joystick::Direction(){
  
  angle =  atan2(coordenada.y.joystickValueFixed,coordenada.x.joystickValueFixed);
  
  if(coordenada.x.joystickValueFixed == 0 && coordenada.y.joystickValueFixed == 0){
    direction = 0;
  }else if (angle >= 0 ){
    if (angle <= DEGREES_45) {         
      direction = DERECHA;
    }else if (angle >= DEGREES_135){
      direction = IZQUIERDA;
    }else{
      direction = ARRIBA;}
  }else{
    if (angle >= -DEGREES_45){
      direction = DERECHA;
    }else if (angle <= -DEGREES_135){
      direction = IZQUIERDA;
    }else{
      direction = ABAJO;}
  }
}

void Joystick::Calibration(){
  for (int i = 0; i <= CALIBRATION_VALUE; i++) {

    if(coordenada.x.joystickValue-(coordenada.x.joystickValueOffset+i) == 0){
      coordenada.x.joystickValueOffset = coordenada.x.joystickValueOffset + i;
    }else if(coordenada.x.joystickValue-(coordenada.x.joystickValueOffset-i) == 0){
      coordenada.x.joystickValueOffset = coordenada.x.joystickValueOffset - i;
    }
    
    if(coordenada.y.joystickValue-(coordenada.y.joystickValueOffset+i) == 0){
      coordenada.y.joystickValueOffset = coordenada.y.joystickValueOffset + i;
    }else if(coordenada.x.joystickValue-(coordenada.y.joystickValueOffset-i) == 0){
      coordenada.y.joystickValueOffset = coordenada.y.joystickValueOffset - i;
    }
  }
}

void Joystick::Setup(){
  begin();
  coordenada.x.joystickValue = readADC_SingleEnded(coordenada.x.channel);
  coordenada.y.joystickValue = readADC_SingleEnded(coordenada.y.channel);
  startTime = 0;
  this->Calibration();
  Serial.println("Calibración terminada!");
}





int Joystick::Loop(){
  /* int value; */
  // unsigned long lastcalibration = 0;
  // unsigned long now = millis();

  // if (now - lastcalibration >= CALIBRATION_TIME) {
  //   this->Calibration();
  //   lastcalibration = now;
  // }
	if ( (millis()-startTime) > DEBOUNCE_TIME ) {
    startTime = millis();
    
    // Leo por I2C la posición x e y del Joystick
    coordenada.x.joystickValue = readADC_SingleEnded(coordenada.x.channel);
    coordenada.y.joystickValue = readADC_SingleEnded(coordenada.y.channel);

  
    // Coloco en el origen el joystick
    coordenada.x.joystickValueFixed = coordenada.x.joystickValue - coordenada.x.joystickValueOffset;
    coordenada.y.joystickValueFixed = coordenada.y.joystickValue - coordenada.y.joystickValueOffset;


  
    if(abs(coordenada.x.joystickValueFixed - coordenada.x.previousJoystickValue) > DIFFERENCE_ERROR_VALUE_MAX){
      coordenada.x.previousJoystickValue = coordenada.x.joystickValueFixed;
    }

    if(abs(coordenada.y.joystickValueFixed - coordenada.y.previousJoystickValue) > DIFFERENCE_ERROR_VALUE_MAX){
      coordenada.y.previousJoystickValue = coordenada.y.joystickValueFixed;
    }

    if(abs(coordenada.x.joystickValueFixed) < DIFFERENCE_ERROR_VALUE_MIN){
      coordenada.x.joystickValueFixed = 0;
      coordenada.x.previousJoystickValue = coordenada.x.joystickValueFixed;
      // Serial.println("x = 0");
    }

    if(abs(coordenada.y.joystickValueFixed) < DIFFERENCE_ERROR_VALUE_MIN){
      coordenada.y.joystickValueFixed = 0;
      coordenada.y.previousJoystickValue = 0;
      coordenada.y.previousJoystickValue = coordenada.y.joystickValueFixed;
      // Serial.println("y = 0");
    }

    // Serial.printf("joystick x value: %d \n", coordenada.x.joystickValueFixed);
    // Serial.printf("joystick y value: %d \n", coordenada.y.joystickValueFixed);

    this->Direction();
  
    if(lastdirection == direction){
      /* value = -1; */
      return -1;    
    }else{
      lastdirection = direction;
      return direction;
      /* value = direction; */
    }
  }
  /* return value; */
}



