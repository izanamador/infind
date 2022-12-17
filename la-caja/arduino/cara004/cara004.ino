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
char *strTopicCfg = "II3/ESP004/cfg/cara004"; // topic para recibir parametros de configuracion
char *strTopicCmd = "II3/ESP004/cmd/cara004"; // topic para recibir peticiones de comando

String teststr = "rhyloo";

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

  /* Initialize joystick*/
  joystick.Setup();
}


void loop(void){
  static int direction = 0;
  display.clearDisplay();
  objInfra.Loop();
  direction = joystick.Loop();

  if (Serial.available() > 0) {
    teststr = Serial.readString();
    teststr.trim(); /* remove any \r \n whitespace at the end of the String */
  }

  /****************************************************************************/
  /* Por alguna extraña razón, la placa se bloquea si las siguientes líneas   */
  /*    de codigo si hay código de por medio antes del print, no estoy muy    */
  /*    seguro  del problema, mientras escribía esto he pensado en el hecho   */
  /*    de que la clase Joystick hereda de una libreria que usa la            */
  /*    comunicación I2C, puede ocurrir algo parecido                         */
  /****************************************************************************/
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  /****************************************************************************/
  display.print(direction);
  display.display();
}
