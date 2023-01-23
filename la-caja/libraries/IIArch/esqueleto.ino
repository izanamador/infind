// 1 Declaración de objetos de arquitectura
#include "IIArch.h"

// 1a     Primer nivel (placa)
IIWifi    obWifi;
IIMqtt    obMqtt;

// 1b     Segundo Nivel (juego)
IIGame    obNumpad;
IIGame    obSwitch;

// 1c     Tercer Nivel (componentes)
IIButton  obFlash; 
IIDht11   obDht11;
IILed     obLedAzul;
IILed     obLedRojo;

// 2 Inclusión de otras librerías específicas del juego
// ...

// 3 Callback de mqtt
void procesa_mensaje(char* strTopicMsg, byte* payload, unsigned int length) {
  bool blMqtt = obMqtt.Message(strTopicMsg, payload, length);  
}



void setup() {

    Serial.begin(115200);

    // 4a Invocación a Setup de objetos a nivel placa
    obWifi.Setup();
    obMqtt.Setup(&obWifi, procesa_mensaje);

    // 4b Invocación a Setup de objetos a nivel juego
    obNumpad.Setup(&obMqtt);
    obSwitch.Setup(&obMqtt);
    
    // 4c Invocación a Setup de objetos a nivel componente
    obFlash.Setup(<pin del botón de flash>);
    obDht11.Setup(<pin donde está conectado el sensor>);
    obLedAzul.Setup(<pin donde está conectado el ese led>);
    //...

    // 5 Inicializaciones específicas del juego
    // ... 
}


void loop() {
  // 6 cesión de control a cada uno de los objetos
  bool hcWifi  = obWifi.HasChanged();
  bool hcMqtt  = obMqtt.HasChanged();
  bool hcDht11 = obDht11.HasChanged();
  bool hcLed   = obLed.HasChanged();

  // 7 Procesamiento específico dependiendo de la necesidad funcional de cada juego
  bool hcNumpad  = obNumpad.HasChanged();
  if (hcNumpad) {
    // ...
  }
  bool hcSwitch  = obSwitch.HasChanged();
  if (hcSwitch) {
    // ...
  }
  // ...
}
