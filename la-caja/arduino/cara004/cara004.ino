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


/* Create display */
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT);

/* Create joystick */
Joystick joystick(x_channel,y_channel);

/* Create objInfra */
Infra objInfra;
char *strTopicPub = "II3/ESP004/pub/cara004"; // topic principal para publicar contenido y lastwill
char *strTopicCmd = "II3/ESP004/cmd/cara004"; // topic para recibir peticiones de comando


/* Resupuesta del acertijo */
int ans = NULL;
String hour = "00:00";

#define MESSAGE_SIZE_ 300
StaticJsonDocument<MESSAGE_SIZE_> json_recibido;

void mqttCallback(char* topic, byte* payload, unsigned int length)
{
  String command, commandValue; 
  char *mensaje = (char *)malloc(length+1);
  strncpy(mensaje, (char*)payload, length);
  mensaje[length]='\0';
  Serial.printf("Mensaje recibido [%s] %s\n", topic, mensaje);

  if (strcmp(topic, strTopicCmd)==0)
    {
      deserializeJson(json_recibido,mensaje);
      hour = json_recibido["content"].as<String>(); // leo de JSON
      ans = json_recibido["ans"]; // leo de JSON
      objInfra.ReportStart(NULL);
    }
  
  free(mensaje);
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

int countDigit(int number){
  /* Calcula el número de dígitos que tiene un número cualquiera */
  return int(log10(number) + 1);
}


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
