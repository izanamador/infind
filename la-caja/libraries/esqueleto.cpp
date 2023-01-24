// 1 includes de la arquitectura
#include <infra.h>
#include <ArduinoJson.h>

// 2 includes específicos del juego
// ...

// 3 Declaración del objeto de la arquitectura
Infra objInfra;

// 4 Resto de declaraciones específicas del juego
// ...

// 5 Inicialización de nombres de topic para publicar y recibir
char *strTopicPub = "II3/ESP002/pub/cara002";
char *strTopicCmd = "II3/ESP002/cmd/cara002";

// 6 Resto de inicializaciones específicas del juego
// ...

void setup(){
  // 7 Setup de la arquitectura
  objInfra.mqttTopicsPub[TOPIC_MAIN] = strTopicPub;
  objInfra.mqttTopicsSub[TOPIC_NUM_CMD] = strTopicCmd;
  objInfra.Setup(mqttCallback);

  // 8 Setup específico del juego
  // ...
}

void mqttCallback(char* topic, byte* payload, unsigned int length){
  // 9 Callback de recepción de mensajes
  char *mensaje = (char *)malloc(length+1);
  strncpy(mensaje, (char*)payload, length);
  mensaje[length]='\0';
  
  if (strcmp(topic, strTopicCmd)==0){
  // 10 Tratamiento del juego al mensaje recibido
    ans = atoi(mensaje);
    objInfra.ReportStart(NULL);}

  // 11 liberación de mememoria del heap
  free(mensaje);
}


void loop(){
  // 12 Declaraciones e inicializaciones antes de la arquitectura
  // ...

  // 13 Cesión del control a la arquitectura
  objInfra.Loop();

  // 14 Si el juego no está activo no hacer nada más
  if (!objInfra.GameRunning())
    return;

  // 15 Código específico del juego
  // ...
  // 16 Si se gana el juego reportar
    objInfra.ReportSuccess(" "); 
  // 17 Si se pierde el juego reportar
    objInfra.ReportFailure(" ");
    // 18 codigo de inicialización de nuevo intento
    // ...

  // 20 Si hay algo relevante para el usuario reportar
    objInfra.ReportStatus2(strDigits);  
}
