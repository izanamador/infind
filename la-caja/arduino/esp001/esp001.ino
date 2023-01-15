#include "EspInfInd.h"
#include "JuegoInfInd.h"

//bool bStandard = true;
bool bStandard = false; 
EspInfInd     oEspInfInd("esp001", bStandard);

//--------- Juegos en el esp001
JuegoInfInd oNumpad("Numpad", 2, &oEspInfInd); 
#include   "numpad/numpad.cpp" // el include contiene la lógica del juego y se debe declarar después de crear el objeto

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

char *strGamedata;

void loop(){
  oEspInfInd.Loop();
  if (oNumpad.GameRunning()) {
    strGamedata = fNumpad(); 
    if (strGamedata != NULL) {
      Serial.println(strGamedata);
      // hay que refrescar la pantalla: copiar la secuencia y enviarla a Nodered
      strcpy(oNumpad.gamedata, strGamedata);
      oNumpad.ReportStatus(oNumpad.gameinfo);
    }
    
  }
    
}
