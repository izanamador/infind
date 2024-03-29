/* --------------------------------------------------------------------------------------- */
#include <infra.h>  /* Declaración de Infra */
#include <Keypad.h> /* Libreria para usar el numpad */
#define NUM_ROWS 4  /* Tamaño del numpad (4 x 4)    */
#define NUM_COLS 4
#define CHAR_0 '0'
#define CHAR_9 '9'
#define CHAR_SEND '#'
#define MAX_DIGITS 9

char keys[NUM_ROWS][NUM_COLS] = {{'1','2','3','A'},  {'4','5','6','B'},  {'7','8','9','C'},  {'*','0','#','D'}};  /* Layout */
byte rowPins[NUM_ROWS] = {D0, D1, D2, D3}; /* Configuración de pines del numpad */
byte colPins[NUM_COLS] = {D4, D5, D6, D7}; /* Configuración de pines del numpad */
Keypad myKeypad = Keypad( makeKeymap(keys), rowPins, colPins, NUM_ROWS, NUM_COLS); /* Instancia de la clase keypad */

Infra objInfra;
char *strTopicPub = "II3/ESP002/pub/cara002"; /* topic principal para publicar contenido y lastwill */
char *strTopicCmd = "II3/ESP002/cmd/cara002"; /* topic para recibir peticiones de comando */

/* VARIABLES GLOBALES */
int ans = NULL; /* Resupuesta del acertijo */

/* --------------------------------------------------------------------------------------- */

/* FUNCIONES */
void mqttCallback(char* topic, byte* payload, unsigned int length){  /* Procesa los mensajes enviados a través Node-red */
  char *mensaje = (char *)malloc(length+1);
  strncpy(mensaje, (char*)payload, length);
  mensaje[length]='\0';
  if (strcmp(topic, strTopicCmd)==0){
    ans = atoi(mensaje);
    objInfra.ReportStart(NULL);}
  free(mensaje);
}


int countDigit(int number){  /* Calcula el número de dígitos que tiene un número cualquiera */
  return int(log10(number) + 1);
}

void setup(){
  /* Setup de la infraestructura */
  objInfra.mqttTopicsPub[TOPIC_MAIN] = strTopicPub;
  objInfra.mqttTopicsSub[TOPIC_NUM_CMD] = strTopicCmd;
  objInfra.Setup(mqttCallback);
}

/* --------------------------------------------------------------------------------------- */

void loop(){
  /* variables */
  static int number = 0;
  static char strDigits[10]= "";
  static int iDigit = 0;

  objInfra.Loop();

  if (!objInfra.GameRunning())
    return;

  char key = myKeypad.getKey(); /* Recibo una tecla del numpad */
  
  if ((key == CHAR_SEND) && (ans == number)){    /* si se pulsa enviar y la respuesta es correcta */
    objInfra.ReportSuccess(" "); /* Actualiza el estado a: "You won" */    
  }
  else if ((key == CHAR_SEND) && (ans != number)){    /* si se pulsa enviar y la respuesta es incorrecta */
    objInfra.ReportFailure(" "); /* Actualiza el estado a: "Failure" y suma un intento */
    strcpy(strDigits,"");        /* Limpio el string en Node-Red */
    number = 0; /* Reinicio el número */
  }
  else if (key >= CHAR_0 && key <= CHAR_9){    /* Envía el número a Node-Red */
    if(countDigit(number) < MAX_DIGITS){
      /* Evita el overflow */
      number = 10*number + (key-48); /* Concateno dígito a digíto para formar un número */
    }
    String str = String(number);
    str.toCharArray(strDigits, 10);
    objInfra.ReportStatus2(strDigits);
  }
}
