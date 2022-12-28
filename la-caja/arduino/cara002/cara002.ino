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

#define n_rows 4
#define n_cols 4

char keys[n_rows][n_cols] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[n_rows] = {D0, D1, D2, D3};
byte colPins[n_cols] = {D4, D5, D6, D7};

/* Create keypad */
Keypad myKeypad = Keypad( makeKeymap(keys), rowPins, colPins, n_rows, n_cols);

/* Create objInfra */
Infra objInfra;
char *strTopicPub = "II3/ESP002/pub/cara002"; // topic principal para publicar contenido y lastwill
char *strTopicCfg = "II3/ESP002/cfg/cara002"; // topic para recibir parametros de configuracion
char *strTopicCmd = "II3/ESP002/cmd/cara002"; // topic para recibir peticiones de comando


int game_ans = NULL;
int state = 0;

void mqttCallback(char* topic, byte* payload, unsigned int length)
{
  char *mensaje = (char *)malloc(length+1);
  strncpy(mensaje, (char*)payload, length);
  mensaje[length]='\0';
  Serial.printf("Mensaje recibido [%s] %s\n", topic, mensaje);

  if (strcmp(topic, strTopicCmd)==0)
    {
      game_ans = atoi(mensaje);
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


/* Codificacion de estados */
/* STATES */
/* Waiting : 0 */
/* InGame : 1 */
/* Finished : 2 */


void SendState(){
  char *msg = "{\"state\": \"\"}";
  switch(state) 
    {
    case 0:
      msg = "{\"state\": \"waiting\"}";
      break;
    case 1:
      msg = "{\"state\": \"ingame\"}";
      break;
    case 2:
      msg = "{\"state\": \"finished\"}";      
      break;
    default:
      Serial.println("States failed!");
    }
  
  // objInfra.MqttPublish(msg);
}

int count_digit(int number) {
  return int(log10(number) + 1);
}

#define MESSAGE_SIZE_ 300

void loop() {
  StaticJsonDocument<MESSAGE_SIZE_> json;
  static char message[MESSAGE_SIZE_];
  
  static int digits = 0;
  static int number = 0;
  static int intentos = 0;
  char *msg;
  
  objInfra.Loop();

  if(state == 0){/* Waiting */
    if(game_ans != NULL){
      digits = count_digit(game_ans);
      state = 1;
    }
  }else if(state == 1){/* InGame */

    char key = myKeypad.getKey();
    
    if (key != NULL && key >= 48 && key <= 59){
      
      number = 10*number + (key-48);
      if(count_digit(number) == digits){
        Serial.println(number);

        if (game_ans == number){
          
          Serial.println("You win!");
          state = 2;
          
        }else{
          intentos++;
          number = 0;
        }        
      }
    }
  }else if(state == 2){/* Finishes */
    json["state"] = "finished";
    json["intentos"] = intentos+1;
    json["tiempo"] = "3600";

    serializeJson(json,message);
    objInfra.MqttPublish(message);
    Serial.println("El juego ha terminado!");
    state = 0;
  }else{
    Serial.println("States failed!");
  }

  SendState();
}


/* Bugs*/
/* Si con dos dígitos intento escribir 01 o 02 no funciona porque */
/* la función count calcula el log10 y no va bien por ahora me funciona */
/* pero tendré que cambiarlo en el futuro */
/* int count_digit(int number) { */
/*    int count = 0; */
/*    while(number != 0) { */
/*       number = number / 10; */
/*       count++; */
/*    } */
/*    return count; */


/* Features */
/* Activar una pista clásica en un texto del dashboard + o - caliente o frío*/
/* Explicar un poco el acertijo, lo cual tiene más trabajo por parte del desarrollador */


