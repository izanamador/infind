 /*******************************************************
Enter the next value of a discrete equation sequence of an 
IQ test in binary by means of switches and a button.
The possible number to represent will be 2^n-1, 
thus having for n = 5 switches, the representation 
of the range [0-31].
The button will be used to send the correct answer.

This version is only an example with the following problem:
Secuence: 3 5 7 11 13 
Correct number: 17
********************************************************/
/* Librería general de la caja  */
#include <infra.h>
#include <ArduinoJson.h>

// Include Libraries
//#include "Arduino.h"
#include <Button.h>
Infra objInfra;
char *strTopicPub = "II3/ESP005/pub/cara005"; // topic principal para publicar contenido y lastwill
//char *strTopicCfg = "II3/ESP005/cfg/cara005"; // topic para recibir parametros de configuracion
char *strTopicCmd = "II3/ESP005/cmd/cara005"; // topic para recibir peticiones de comando
// Variables para enviar y recibir datos por MQTT
#define MESSAGE_SIZE_ 300
static char message[MESSAGE_SIZE_];
StaticJsonDocument<MESSAGE_SIZE_> json;




// Pin Definitions
#define PUSHBUTTON_PIN_2  15
#define TOGGLESWITCH_1_PIN_2  4
#define TOGGLESWITCH_2_PIN_2  10
#define TOGGLESWITCH_3_PIN_2  2
#define TOGGLESWITCH_4_PIN_2  14
#define TOGGLESWITCH_5_PIN_2  12

// Global variables and defines

// object initialization
Button pushButton(PUSHBUTTON_PIN_2);
Button ToggleSwitch_1(TOGGLESWITCH_1_PIN_2);
Button ToggleSwitch_2(TOGGLESWITCH_2_PIN_2);
Button ToggleSwitch_3(TOGGLESWITCH_3_PIN_2);
Button ToggleSwitch_4(TOGGLESWITCH_4_PIN_2);
Button ToggleSwitch_5(TOGGLESWITCH_5_PIN_2);


void setup() 
{
    //    Serial.begin(9600);
    //while (!Serial) ; // wait for serial port to connect. Needed for native USB
    //Serial.println("start");
    pushButton.init();
    ToggleSwitch_1.init();
    ToggleSwitch_2.init();
    ToggleSwitch_3.init();
    ToggleSwitch_4.init();
    ToggleSwitch_5.init();

    /* setup de infrastructura */
    objInfra.mqttTopicsPub[TOPIC_MAIN] = strTopicPub;
  //  objInfra.mqttTopicsSub[TOPIC_NUM_CFG] = strTopicCfg;
    objInfra.mqttTopicsSub[TOPIC_NUM_CMD] = strTopicCmd;
    objInfra.Setup(mqttCallback);
}


//define vars for program
int GameAns = 17;

void mqttCallback(char* topic, byte* payload, unsigned int length)
{
  char *mensaje = (char *)malloc(length+1);
  strncpy(mensaje, (char*)payload, length);
  mensaje[length]='\0';
  Serial.printf("Mensaje recibido [%s] %s\n", topic, mensaje);

  if (strcmp(topic, strTopicCmd)==0)
    {
      GameAns = atoi(mensaje);
      objInfra.ReportStart(NULL);
    }
  Serial.println(GameAns);
  free(mensaje);
}


void loop() 
{
  static char strSwitches[]="_____";
  objInfra.Loop();
  if (!objInfra.GameRunning())
    return;

  bool pushButtonVal = pushButton.read();
  std::array<int, 5> vectorSwitches {
    ToggleSwitch_1.read(),
    ToggleSwitch_2.read(),
    ToggleSwitch_3.read(),
    ToggleSwitch_4.read(),
    ToggleSwitch_5.read()};
    
        

  int enteroEquivalente = 0;
  
  for (int i = 0; i < 5; i++) {
    enteroEquivalente += vectorSwitches[i] << i;
    strSwitches[i] = vectorSwitches[i] ? '1' : '0';
  }

  Serial.println(enteroEquivalente);
  if(pushButtonVal == HIGH) {
    if(enteroEquivalente == GameAns) {
      objInfra.ReportSuccess(strSwitches);
      Serial.println(F("¡Ganaste! Felicidades Tienes 150 de IQ"));
    }       
    else
      objInfra.ReportFailure(strSwitches);
  }
}
