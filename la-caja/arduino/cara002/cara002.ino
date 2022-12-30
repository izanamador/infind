/* Librería general de la caja  */
#include <infra.h>
#include <ArduinoJson.h>

/* Librerías de uso generales*/
#include <array>
#include <string.h>
#include <Wire.h>
#include <math.h>

/* Libreria keypad */
#include <Keypad.h>

#define NUM_ROWS 4
#define NUM_COLS 4
#define MESSAGE_SIZE_ 300

char keys[NUM_ROWS][NUM_COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[NUM_ROWS] = {D0, D1, D2, D3};
byte colPins[NUM_COLS] = {D4, D5, D6, D7};

/* Create keypad */
Keypad myKeypad = Keypad( makeKeymap(keys), rowPins, colPins, NUM_ROWS, NUM_COLS);

/* Create objInfra */
Infra objInfra;
char *strTopicPub = "II3/ESP002/pub/cara002"; // topic principal para publicar contenido y lastwill
char *strTopicCfg = "II3/ESP002/cfg/cara002"; // topic para recibir parametros de configuracion
char *strTopicCmd = "II3/ESP002/cmd/cara002"; // topic para recibir peticiones de comando

/* Resupuesta del acertijo */
int game_ans = NULL;
static char message[MESSAGE_SIZE_];
StaticJsonDocument<MESSAGE_SIZE_> json;

void mqttCallback(char* topic, byte* payload, unsigned int length)
{
  char *mensaje = (char *)malloc(length+1);
  strncpy(mensaje, (char*)payload, length);
  mensaje[length]='\0';
  Serial.printf("Mensaje recibido [%s] %s\n", topic, mensaje);

  if (strcmp(topic, strTopicCmd)==0)
    {
      game_ans = atoi(mensaje);
      json["state"] = 1;
      json["intentos"] = 0;
      json["tiempo"] = 0;
      json["clock"] = 1;
      serializeJson(json,message);
      objInfra.MqttPublish(message);
    }
  Serial.println(game_ans);
  free(mensaje);
}

void setup() {
  /* setup de infrastructura */
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

/* --------------------------------------------- GAME */
/* MACROS */
#define CHAR_0 '0'
#define CHAR_9 '9'
#define CHAR_SEND '#'
#define MAX_DIGITS 9


/* VARIABLES GLOBALES */
int state = 0;

void loop() {

  /* VARIABLES */

  static int una_vez = 0;
  static int number = 0;
  static int intentos = 0;
  static unsigned long ultimo_mensaje = 0;
  char *msg;
  
  objInfra.Loop();
  unsigned long ahora = millis();

  
  if(state == 0){/* WAITING -- Espero hasta que reciba la respuesta a comparar por MQTT */
    json["clock"] = 0;
    if(game_ans != NULL){
      state = 1;
    }
  }else if(state == 1){/* INGAME -- Estoy en el juego hasta que el usuario acierte */
    json["clock"] = 1;
    char key = myKeypad.getKey(); /* Recibo una tecla del numpad */
    
    if ((key != NULL && key >= CHAR_0 && key <= CHAR_9) || key == CHAR_SEND){/* Compruebo si es válida (0,1,..,9, #)*/
      
      if(key != CHAR_SEND){
        if(CountDigit(number) < MAX_DIGITS){ /* Evito el overflow */
          number = 10*number + (key-48); /* Concateno dígito a digíto para formar un número */
        }

        json["numero"] = number;
        serializeJson(json,message);
        objInfra.MqttPublish(message);        
          
      }else{                    /* Si el usuario le ha dado a enviar compruebo si ha acertado */
        json["numero"] = " ";   /* Limpio el dashboard, da la sensación de que ha sido enviado */
        serializeJson(json,message);
        objInfra.MqttPublish(message);        

        Serial.println(number);
        
        if (game_ans == number){ /* Comparo si la respuesta del usuario coincide con la correcta */
          
          Serial.println("You win!");
          state = 2;
          
        }else{
          intentos++;           /* Aumento el número de intentos hechos por el usuario */
          number = 0;           /* Reinicio el número */
        }        
      }
    }
  }else if(state == 2){/* FINISHED -- Envio los resultados en un JSON*/
    json["state"] = state;
    json["intentos"] = intentos+1;
    json["tiempo"] = "3600";
    json["clock"] = 0;
    serializeJson(json,message);
    if(una_vez == 0){
    objInfra.MqttPublish(message);
    una_vez = 1;
    }
    Serial.println("El juego ha terminado!");
    
    game_ans = NULL;    
  }else{
    Serial.println("States failed!"); /* Por si el juego falla en algún momento, es una medida de seguridad */
  }
  
  if (ahora - ultimo_mensaje >= 1000 && state !=2) {
    SendState();
  }
  
}

/****************************/
/* Declaración de funciones */
/****************************/
void SendState(){
  /* Selecciona un estado del juego y compone un JSON para enviarlo 
     como mensaje a Node-Red*/
  /********** STATES *********/
  /* WAITING  : 0            */
  /* INGAME   : 1            */
  /* FINISHED : 2            */
  /***************************/
  
  json["state"] = state;

  /* Desactivado hasta que nos pogamos de acuerdo con el envío de este tipo de datos*/
  serializeJson(json,message);
  objInfra.MqttPublish(message);
}

int CountDigit(int number) {
  /* Calcula el número de dígitos que tiene un número cualquiera */
  return int(log10(number) + 1);
}
