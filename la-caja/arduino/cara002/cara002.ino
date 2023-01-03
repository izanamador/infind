/* Librería general de la caja  */
#include <infra.h>
#include <ArduinoJson.h>

// Variables para enviar y recibir datos por MQTT
#define MESSAGE_SIZE_ 300
static char message[MESSAGE_SIZE_];
StaticJsonDocument<MESSAGE_SIZE_> json;
  

/* Librerías de uso generales*/
#include <array>
#include <string.h>
#include <Wire.h>
#include <math.h>


/* Create objInfra */
// TODO Estandarizar los nombres de topics en la infrasestructura
// Ejemplo de topic que cumple 
// #define TOPIC_PUB_ "II3/ESP14440037/resultados_juego2"

Infra objInfra;
char *strTopicPub = "II3/ESP002/pub/cara002"; // topic principal para publicar contenido y lastwill
char *strTopicCfg = "II3/ESP002/cfg/cara002"; // topic para recibir parametros de configuracion
char *strTopicCmd = "II3/ESP002/cmd/cara002"; // topic para recibir peticiones de comando


/* Libreria keypad */
#include <Keypad.h>

// Tamaño del numpad (4 x 4) y layout
#define NUM_ROWS 4
#define NUM_COLS 4
char keys[NUM_ROWS][NUM_COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};


  

// Configuración de pines del numpad y creación de instancia de la clase
byte rowPins[NUM_ROWS] = {D0, D1, D2, D3};
byte colPins[NUM_COLS] = {D4, D5, D6, D7};
Keypad myKeypad = Keypad( makeKeymap(keys), rowPins, colPins, NUM_ROWS, NUM_COLS);



void setup() {
  /* setup de infrastructura */
  objInfra.mqttTopicsPub[TOPIC_MAIN] = strTopicPub;
  objInfra.mqttTopicsSub[TOPIC_NUM_CFG] = strTopicCfg;
  objInfra.mqttTopicsSub[TOPIC_NUM_CMD] = strTopicCmd;
  objInfra.Setup(mqttCallback);
}



/* Resupuesta del acertijo */
int game_ans = NULL;

void mqttCallback(char* topic, byte* payload, unsigned int length)
{
  char *mensaje = (char *)malloc(length+1);
  strncpy(mensaje, (char*)payload, length);
  mensaje[length]='\0';
  Serial.printf("Mensaje recibido [%s] %s\n", topic, mensaje);

  if (strcmp(topic, strTopicCmd)==0)
    {
      game_ans = atoi(mensaje);
      objInfra.ReportStart(NULL);
    }
  Serial.println(game_ans);
  free(mensaje);
}



/* --------------------------------------------- GAME */
/* MACROS */
#define CHAR_0 '0'
#define CHAR_9 '9'
#define CHAR_SEND '#'
#define MAX_DIGITS 9



void loop() {

  /* VARIABLES */

  static int una_vez = 0;
  static int number = 0;
  static int intentos = 0;
  static unsigned long ultimo_mensaje = 0;
  char *msg;
  char strDigits[10]="    ";
  static int iDigit = 0;

  objInfra.Loop();

  if (!objInfra.GameRunning())
    return;

  char key = myKeypad.getKey(); /* Recibo una tecla del numpad */  

  // si se pulsa enviar y la respuesta es correcta
  if ((key == CHAR_SEND) && (game_ans == number)) 
  {
    Serial.println(number);
    objInfra.ReportSuccess(" ");
    Serial.println("You win!");
  }

  // si se pulsa enviar y la respuesta es incorrecta
  else if ((key == CHAR_SEND) && (game_ans != number))
  {
    Serial.println(number);
    objInfra.ReportFailure(" ");
    number = 0;           /* Reinicio el número */
    strcpy(strDigits,"     ");
    iDigit = 0;
  }

  // el jugador ha pulsado una tecla numérica disinta de SEND
  // TODO REVISAR SI EL PRIMER TÉRMINO REALMENTE HACE FALTA (KEY!=NULL)
  else if ((key != NULL) && (key >= CHAR_0 && key <= CHAR_9))
  {
    // dar feedback de la tecla que ha pulsado
    if(CountDigit(number) < MAX_DIGITS){ /* Evito el overflow */
      number = 10*number + (key-48); /* Concateno dígito a digíto para formar un número */
    }
    strDigits[iDigit++]=key;
    objInfra.ReportStatus(strDigits);
    //sprintf(strAux, "%d", number);
    //objInfra.ReportStatus(strAux);     
  }

  
}

/****************************/
/* Declaración de funciones */
/****************************/

int CountDigit(int number) {
  /* Calcula el número de dígitos que tiene un número cualquiera */
  return int(log10(number) + 1);
}
