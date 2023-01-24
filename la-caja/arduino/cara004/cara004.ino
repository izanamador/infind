/* --------------------------------------------------------------------------------------- */
#include <Adafruit_ADS1X15.h> /* Libreria convertidor A/D ADS1015 */
#include <Adafruit_SSD1306.h> /* Librerías Pantalla Oled SSD1306 */
#include <Adafruit_GFX.h>
#include <infra.h>  /* Declaración de Infra */
#include <Joystick.h> /* Librería de cosecha propia de la caja*/
#define x_channel 1
#define y_channel 0
#define SCREEN_WIDTH 128 /* Units in px */
#define SCREEN_HEIGHT 64 /* Units in px */
#define MESSAGE_SIZE_ 300

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT); /* Create display */
Joystick joystick(x_channel,y_channel); /* Create joystick */
Infra objInfra;
char *strTopicPub = "II3/ESP004/pub/cara004"; // topic principal para publicar contenido y lastwill
char *strTopicCmd = "II3/ESP004/cmd/cara004"; // topic para recibir peticiones de comando

/* VARIABLES GLOBALES */
int ans = NULL; /* Resupuesta del acertijo */
String hour = "00:00";
StaticJsonDocument<MESSAGE_SIZE_> json_recibido;


/* --------------------------------------------------------------------------------------- */

/* FUNCIONES */
void mqttCallback(char* topic, byte* payload, unsigned int length){ /* Procesa los mensajes enviados a través Node-red */

  char *mensaje = (char *)malloc(length+1);
  strncpy(mensaje, (char*)payload, length);
  mensaje[length]='\0';

  if (strcmp(topic, strTopicCmd)==0){
      deserializeJson(json_recibido,mensaje);
      hour = json_recibido["content"].as<String>(); // leo de JSON
      ans = json_recibido["ans"]; // leo de JSON
      objInfra.ReportStart(NULL);
    }
  free(mensaje);
}

int countDigit(int number){ /* Calcula el número de dígitos que tiene un número cualquiera */
  return int(log10(number) + 1);
}

void setup(void){
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }

  /* setup de infrastructura */
  objInfra.mqttTopicsPub[TOPIC_MAIN] = strTopicPub;
  objInfra.mqttTopicsSub[TOPIC_NUM_CMD] = strTopicCmd;
  objInfra.Setup(mqttCallback);

  joystick.Setup();
  
  display.clearDisplay();
  display.setTextSize(4);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(5, 20);

  display.print(hour);
  display.display();
}

/* --------------------------------------------------------------------------------------- */

void loop(){
  /* variables */
  int joystick_value;
  static int number = 0;
  static char strDigits[10]= "";
  static int iDigit = 0;

  display.clearDisplay();       /* Refresca la pantalla */
  objInfra.Loop();              /* Loop infrastructura */

  if (!objInfra.GameRunning())
    return;

  /* Pantalla */
  display.setTextSize(4);       /* Tamaño del texto - OLed */
  display.setTextColor(SSD1306_WHITE); /* Color del texto */
  display.setCursor(5, 20);            /* Mover el cursor */
  display.print(hour);                 /* Imprime el texto por pantalla */
  display.display();
  
  joystick_value = joystick.Loop(); /* Joystick Loop */
  
  if ((countDigit(number) == 4) && (ans == number)){
    /* si la respuesta es correcta */
    objInfra.ReportSuccess(" "); /* Actualiza el estado a: "You won" */    
  }
  else if ((countDigit(number) == 4) && (ans != number)){
    /* si la respuesta es incorrecta */
    objInfra.ReportFailure(" "); /* Actualiza el estado a: "Failure" y suma un intento */
    strcpy(strDigits,"");        /* Limpio el string en Node-Red */
    number = 0; /* Reinicio el número */
  }
  else if(joystick_value != -1){
      number = 10*number + joystick_value;
    String str = String(joystick_value);
    str.toCharArray(strDigits, 10);
    objInfra.ReportStatus2(strDigits);
  }
}
