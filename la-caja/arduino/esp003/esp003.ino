#include "EspInfInd.h"
#include "JuegoInfInd.h"
bool bStandard = true;
EspInfInd     oEspInfInd("esp003", bStandard);

//--------- Juegos en el esp003
JuegoInfInd oJoystk("Joystk", 4, &oEspInfInd); 
JuegoInfInd oCrono ("Crono" , 1, &oEspInfInd);


//------------------------------------------ Funciones del juego Joystk
void fJoystk() {
}

//------------------------------------------ Funciones del juego Crono
void fCrono() {
}

//------------------------------------------------------------------- ESP

void mqttCallback(char* topic, byte* payload, unsigned int length){
  oEspInfInd.MqttReceived(topic, payload, length);
}

void setup() {
    delay(1000);
    pinMode(LED_BUILTIN, OUTPUT);
    oEspInfInd.Setup(mqttCallback);
    delay(2000);
}

void loop() {
  oEspInfInd.Loop();
  if (oJoystk.GameRunning()) {
    fJoystk();
  }
  if (oCrono.GameRunning()) {
    fCrono();
  }
}

