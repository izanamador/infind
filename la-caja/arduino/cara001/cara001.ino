#include <infra.h>

Infra objInfra;
char *strTopicPub = "la-caja/cara001/pub";
char *strTopicCfg = "la-caja/cara001/cfg";
char *strTopicCmd = "la-caja/cara001/cmd";

void mqttCallback(char* topic, byte* payload, unsigned int length) 
{
  char *mensaje = (char *)malloc(length+1); 
  strncpy(mensaje, (char*)payload, length); 
  mensaje[length]='\0'; 
  Serial.printf("Mensaje recibido [%s] %s\n", topic, mensaje);
  
  if (strcmp(topic, strTopicCmd)==0) 
  {
    if (mensaje[0] == '1') {
        digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level 
      } 
      else 
      {
        digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
      }
  }
  free(mensaje);
}


void setup() {
  // setup de infrastructura
  objInfra.mqttTopicPub = strTopicPub;
  objInfra.mqttTopicsSub[TOPIC_NUM_CFG] = strTopicCfg;
  objInfra.mqttTopicsSub[TOPIC_NUM_CMD] = strTopicCmd;
  objInfra.Setup(mqttCallback);

}

void loop() {
  // loop de infrastructura
  objInfra.Loop();

}
