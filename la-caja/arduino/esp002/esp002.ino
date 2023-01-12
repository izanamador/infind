#include "EspInfInd.h"
#include "JuegoInfInd.h"
bool bStandard = true;
EspInfInd     oEspInfInd("esp002", bStandard);

//--------- Juegos en el esp002
JuegoInfInd  oSimon("Simon" , 3, &oEspInfInd); 

//------------------------------------------ Funciones del juego Simon
void fSimon() {
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
  if (oSimon.GameRunning()) {
    fSimon();
  }
}

