#include "EspInfInd.h"

bool bStandard = true;
EspInfInd oEspInfInd("esp001", bStandard);

void mqttCallback(char* topic, byte* payload, unsigned int length){
  oEspInfInd.MqttReceived(topic, payload, length);
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    delay(1000);
    oEspInfInd.Setup(mqttCallback);
    delay(2000);
}

void loop() {
  static unsigned int msLast=0;
  oEspInfInd.Loop();
  if (msLast+60000<millis()) {
    Serial.printf("Sigo vivo\n");
    msLast=millis();
  }
}
