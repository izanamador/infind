
#include "EspInfInd.h"
#include "JuegoInfInd.h"

//bool bStandard = true;
bool bStandard = false; 
EspInfInd     oEspInfInd("esp001", bStandard);

//--------- Juegos en el esp003
JuegoInfInd oNumpad("Numpad", 2, &oEspInfInd); 



/* Librerías de uso generales*/
#include <array>
#include <string.h>
#include <Wire.h>
#include <math.h>
#include <cstring>
#include <Keypad.h> /* Libreria para usar el numpad */
#define NUM_ROWS 4  /* Tamaño del numpad (4 x 4)    */
#define NUM_COLS 4
char keys[NUM_ROWS][NUM_COLS] = {
  /* Layout */
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[NUM_ROWS] = {D0, D1, D2, D3}; /* Configuración de pines del numpad */
byte colPins[NUM_COLS] = {D4, D5, D6, D7}; /* Configuración de pines del numpad */
Keypad myKeypad = Keypad( makeKeymap(keys), rowPins, colPins, NUM_ROWS, NUM_COLS); /* Instancia de la clase keypad */


void setup(){
     delay(1000);
//     pinMode(LED_BUILTIN, OUTPUT);
     oEspInfInd.Setup(mqttCallback);
     delay(1000);
}

int ans = NULL; /* Resupuesta del acertijo */

void mqttCallback(char* topic, byte* payload, unsigned int length){
  oEspInfInd.MqttReceived(topic, payload, length);
}

static char strReport[50];


int countDigit(int number){
  /* Calcula el número de dígitos que tiene un número cualquiera */
  return int(log10(number) + 1);
}


#define CHAR_0 '0'
#define CHAR_9 '9'
#define CHAR_SEND '#'
#define MAX_DIGITS 9


void loop(){
  //static int number = 0;
  static char strDigits[10]= "";
  static int iDigit = 0;
  oEspInfInd.Loop();

  if (!oNumpad.GameRunning()) return;
  
  char key = myKeypad.getKey(); /* Recibo una tecla del numpad */
  sprintf(strReport, "Ult tecla = %c\n", key);

  if (key == '#') {
    strDigits[iDigit] = '\0'; // nulo al final de la secuencia de teclas pulsadas
    sprintf(strReport, "Respuesta=%s, solucion=%s\n", strDigits, oNumpad.GameParm);    
    if (strcmp(strDigits, oNumpad.GameParm)==0) {
      oNumpad.ReportSuccess(strReport);
    }
    else {
      oNumpad.ReportFail(strReport);
      iDigit = 0; // volver a iniciar la secuencia de números pulsados desde la posición 0
    }        
  }
  
  else if (key >= '0' && key <= '9') {
    if (iDigit <= MAX_DIGITS) {
      strDigits[iDigit++] = key;
    }
    oNumpad.ReportStatus(strDigits);
  }
}
