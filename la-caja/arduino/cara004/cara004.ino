/* Librerías de uso generales*/
#include <array>
#include <string.h>
#include <Wire.h>

/* Libreria convertidor A/D ADS1015 */
#include <Adafruit_ADS1X15.h>

/* Librerías Pantalla Oled SSD1306 */
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

/* Librería general de la caja  */
#include <infra.h>

/* Librería de cosecha propia de la caja*/
#include <Joystick.h>

#define x_channel 1
#define y_channel 0
#define SCREEN_WIDTH 128 /* Units in px */
#define SCREEN_HEIGHT 64 /* Units in px */


#define MESSAGE_SIZE_ 300

#define CLOCK_STOP 0
#define CLOCK_START 1
#define CLOCK_RESET 2
#define CLOCK_PAUSE 3
#define CLOCK_CONTINUE 4

/* Create display */
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT);

/* Create joystick */
Joystick joystick(x_channel,y_channel);

/* Create objInfra */
Infra objInfra;
char *strTopicPub = "II3/ESP004/pub/cara004"; // topic principal para publicar contenido y lastwill
char *strTopicCfg = "II3/ESP004/cfg/cara004"; // topic para recibir parametros de configuracion
char *strTopicCmd = "II3/ESP004/cmd/cara004"; // topic para recibir peticiones de comando


/* Stuff needed */
String teststr = "rhyloo";
/* Resupuesta del acertijo */
int game_ans = NULL;
String hour_ans = "00:00";

static char message[MESSAGE_SIZE_];
StaticJsonDocument<MESSAGE_SIZE_> json;
StaticJsonDocument<MESSAGE_SIZE_> json_recibido;

void mqttCallback(char* topic, byte* payload, unsigned int length)
{
  char *mensaje = (char *)malloc(length+1);
  strncpy(mensaje, (char*)payload, length);
  mensaje[length]='\0';
  Serial.printf("Mensaje recibido [%s] %s\n", topic, mensaje);

  if (strcmp(topic, strTopicCmd)==0)
    {

      deserializeJson(json_recibido,mensaje);
      hour_ans = json_recibido["content"].as<String>(); // leo de JSON
      game_ans = json_recibido["ans"]; // leo de JSON

      json["state"] = 1;        /* Comentar después */
      json["intentos"] = 0;
      json["tiempo"] = 0;
      json["clock"] = CLOCK_START;
      serializeJson(json,message);
      objInfra.MqttPublish(message);
    }
  Serial.println(game_ans);
  free(mensaje);
}

void setup(void){
  /****************************************************************************/
  /* Las siguientes lineas son muy importantes, desconozco el motivo, mi idea */
  /*    es que si no las pones intenta leer el bus I2C más rápido de lo que   */
  /*    puede y peta, o que directamente se lo salta y hace un clean display  */
  /*    al aire.                                                              */
  /****************************************************************************/
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Display setup failed");
    while (true);
  }
  Serial.println("Display is good to go");
  /****************************************************************************/

  /* setup de infrastructura */
  objInfra.mqttTopicsPub[TOPIC_MAIN] = strTopicPub;
  objInfra.mqttTopicsSub[TOPIC_NUM_CFG] = strTopicCfg;
  objInfra.mqttTopicsSub[TOPIC_NUM_CMD] = strTopicCmd;
  objInfra.Setup(mqttCallback);

  /* Initialize joystick*/
  joystick.Setup();
  
  display.clearDisplay();
  display.setTextSize(4);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(5, 20);
  /****************************************************************************/
  display.print(hour_ans);
  display.display();
}

/* --------------------------------------------- GAME */
/* MACROS */
#define CHAR_0 '0'
#define CHAR_9 '9'
#define CHAR_SEND '#'
#define MAX_DIGITS 9


/* VARIABLES GLOBALES */
int state = 0;

void loop(void){

  static int una_vez = 0;
  static int number = 0;
  static int intentos = 0;
  unsigned long ahora = millis();
  static unsigned long ultimo_mensaje = 0;
  char *msg;
  
  display.clearDisplay();
  objInfra.Loop();

  static int actual_direction = 0;
  static int last_direction = 0;

  if(state == 0){/* WAITING -- Espero hasta que reciba la respuesta a comparar por MQTT */
    json["clock"] = CLOCK_STOP;

    if(game_ans != NULL){
      state = 1;
    }
    
  }else if(state == 1){/* INGAME -- Estoy en el juego hasta que el usuario acierte */
      /****************************************************************************/
    /* Por alguna extraña razón, la placa se bloquea si las siguientes líneas   */
    /*    de codigo si hay código de por medio antes del print, no estoy muy    */
    /*    seguro  del problema, mientras escribía esto he pensado en el hecho   */
    /*    de que la clase Joystick hereda de una libreria que usa la            */
    /*    comunicación I2C, puede ocurrir algo parecido                         */
    /****************************************************************************/
    display.setTextSize(4);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(5, 20);
    /****************************************************************************/
    display.print(hour_ans);
    display.display();
    json["clock"] = CLOCK_START;
    
    actual_direction = joystick.Loop();
    if(actual_direction != last_direction){
      number = 10*number + actual_direction;
      json["numero"] = number;
        serializeJson(json,message);
        objInfra.MqttPublish(message);
        last_direction = actual_direction;
      if(CountDigit(number) == 4){ /* Evito el overflow */
        Serial.println(number);
        json["numero"] = " ";
        serializeJson(json,message);
        objInfra.MqttPublish(message);
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
    json["clock"] = CLOCK_STOP;
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
  /* objInfra.MqttPublish(message); */
}

int CountDigit(int number) {
  /* Calcula el número de dígitos que tiene un número cualquiera */
  return int(log10(number) + 1);
}
