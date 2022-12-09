#include <infra.h>

Infra objInfra;
char *strTopicPub = "la-caja/pub/cara001"; // topic principal para publicar contenido y lastwill
char *strTopicCfg = "la-caja/cfg/cara001"; // topic para recibir parametros de configuracion
char *strTopicCmd = "la-caja/cmd/cara001"; // topic para recibir peticiones de comando

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
  objInfra.mqttTopicsPub[TOPIC_MAIN] = strTopicPub;
  objInfra.mqttTopicsSub[TOPIC_NUM_CFG] = strTopicCfg;
  objInfra.mqttTopicsSub[TOPIC_NUM_CMD] = strTopicCmd;
  objInfra.objConfig["CRONO_INI"] ="30:00";
  objInfra.objConfig["CRONO_MAX"] ="90:00";
  objInfra.objConfig["CRONO_ADD"] ="10:00";
  objInfra.objConfig["FREQ_START"]=1200;
  objInfra.objConfig["FREQ_CARA"] =1300;
  objInfra.objConfig["FREQ_FIN"]  =1400;
  objInfra.objConfig["DURA_START"]=5000;
  objInfra.objConfig["DURA_CARA"] =2000;
  objInfra.objConfig["DURA_FIN"]  =10000;

  objInfra.Setup(mqttCallback);

}

void loop() {
  // loop de infrastructura
  objInfra.Loop();

}
