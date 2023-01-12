#include "EspInfInd.h"
#include "JuegoInfInd.h"
bool bStandard = true;
EspInfInd     oEspInfInd("esp004", bStandard);
//--------- Juegos en el esp004
JuegoInfInd oSwitch("Switch", 5, &oEspInfInd); 
JuegoInfInd  oBarco("Barco" , 6, &oEspInfInd);


//------------------------------------------ Funciones del juego Switch
void fSwitch() {
}

//------------------------------------------ Funciones del juego Barco
void fBarco() {
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
  if (oSwitch.GameRunning()) {
    fSwitch();
  }
  if (oBarco.GameRunning()) {
    fBarco();
  }
}
