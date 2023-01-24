#include "EspInfInd.h"
#include "JuegoInfInd.h"

//bool bStandard = true;
bool bStandard = false; 
EspInfInd     oEspInfInd("esp001", bStandard);

//--------- Juegos en el esp001
#include   "numpad/numpad.cpp" // el include contiene la lógica del juego y se debe declarar después de crear el objeto
JuegoNumpad objNumpad("Numpad", 2, &oEspInfInd); 

//--------- Juegos en el esp001
void setup(){
     delay(1000);
//     pinMode(LED_BUILTIN, OUTPUT);
     oEspInfInd.Setup(mqttCallback);
     delay(1000);
}

void mqttCallback(char* topic, byte* payload, unsigned int length){
  oEspInfInd.MqttReceived(topic, payload, length);
}


void loop(){
  oEspInfInd.Loop();
  if (objNumpad.GameRunning()) {
    objNumpad.Loop();    
  }    
}
