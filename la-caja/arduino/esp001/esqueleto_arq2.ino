// 1 Includes de la arquitectura
#include "EspInfInd.h"
#include "JuegoInfInd.h"

// 2 Creación del objeto ESP
// bool bStandard = true;   // descomentar si la arquitectura gestiona los leds
// bool bStandard = false;  // decomentar si la arquitectura omite los leds
EspInfInd     oEspInfInd("esp001", bStandard);

// 3 Delcaraciones específicas de los juegos
// ...

// 4 Declaraciones de objetos de la clase JuegoInfInd
//   En el ejemplo dos juegos, Numpad en la cara 2 y Switch en la cara 4
JuegoInfInd  oNumpad("Numpad" , 2, &oEspInfInd); 
JuegoInfInd  oSwitch("Switch" , 4, &oEspInfInd); 

// 5 Invocación a la arquitectura con cada mensaje recibido
void mqttCallback(char* topic, byte* payload, unsigned int length){
  oEspInfInd.MqttReceived(topic, payload, length);
}

void setup(){
  // 6 Inicializaciones específicas de los juegos
  // ...

  // 7 Inicialización de la arquitectura
     oEspInfInd.Setup(mqttCallback);
}


void loop(){
  // 8 Cesión del control al objeto que gestiona la placa
  oEspInfInd.Loop();

  // 9a Cesión del control al objeto del primer juego
  if (objNumpad.GameRunning()) {
    // 10a Código específico del primer juego
    // ...
  }    

  // 9b Cesión del control al objeto del segundo juego
  if (objSwitch.GameRunning()) {
    // 10b Código específico del segundo juego
    // ...
  }    
  // ... (repetir para tantos juegos como sea necesario)
}


