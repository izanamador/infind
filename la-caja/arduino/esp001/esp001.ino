#include "EspInfInd.h"
#include "JuegoInfInd.h"

//--------- 
bool bStandard = true;
EspInfInd     oEspInfInd("esp001", bStandard);

//--------- Juegos en el esp001
JuegoInfInd oNumpad("Numpad", 2, &oEspInfInd);
JuegoInfInd oPrueba("Prueba", 7, &oEspInfInd); 


//------------------------------------------------------------------- NUMPAD

/* Librerías de uso generales*/
#include <array>
#include <string.h>
#include <Wire.h>
#include <math.h>
#include <cstring>

#include <Keypad.h> /* Libreria para usar el numpad */
#define NUMPAD_MAX_DIGITS 10
#define NUMPAD_NUM_ROWS 4  /* Tamaño del numpad (4 x 4)    */
#define NUMPAD_NUM_COLS 4

char strAux[300];

//------------------------------------------ Funciones del juego Numpad
void fNumpad() {
  static char keys[NUMPAD_NUM_ROWS][NUMPAD_NUM_COLS] = {
    /* Layout */
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
  };
  static byte rowPins[NUMPAD_NUM_ROWS] = {D0, D1, D2, D3}; /* Configuración de pines del numpad */
  static byte colPins[NUMPAD_NUM_COLS] = {D4, D5, D6, D7}; /* Configuración de pines del numpad */
  static Keypad myKeypad = Keypad( makeKeymap(keys), rowPins, colPins, NUMPAD_NUM_ROWS, NUMPAD_NUM_COLS); /* Instancia de la clase keypad */

  // string con el número que se está componiendo hasta pulsar send
  static char strDigits[NUMPAD_MAX_DIGITS];
  static int  iDigit = 0;
  //objInfra.Loop(strDigits);
  char key = myKeypad.getKey(); // TODO CONFIRMAR SI ES BLOQUEANTE
  
  if (key == '#') { // tecla # es la de envío, comprobar si la respuesta es correcta
    strDigits[iDigit] = '\0'; // nulo al final del string
    if (iDigit > 0 && (strcmp(strDigits,oNumpad.GameParm)==0))
    {
      sprintf(strAux, "Numero %s correcto", strDigits);
      oNumpad.ReportSuccess(strAux);

    } else {
      sprintf(strAux, "Numero %s erroneo", strDigits);
      oNumpad.ReportFail(strAux);
      iDigit = 0;
    }
  }

  else if (iDigit == NUMPAD_MAX_DIGITS) {
      sprintf(strAux, "Numero demasiado largo %s", strDigits);
    
      oNumpad.ReportFail(strAux);
    iDigit = 0;

  }

  else if (key >= '0' && key <= '0') {
    strDigits[iDigit++] = key; // actualiza el string y la posición
    strDigits[iDigit] = '\0'; // nulo al final del string
    oNumpad.ReportStatus(strDigits);

  }
}

//------------------------------------------------------------------- ESP

void mqttCallback(char* topic, byte* payload, unsigned int length){
  oEspInfInd.MqttReceived(topic, payload, length);
}

void setup() {
    delay(1000);
    pinMode(LED_BUILTIN, OUTPUT);
    oEspInfInd.Setup(mqttCallback);
    delay(2000);
}

void loop() {
  oEspInfInd.Loop();
  if (oNumpad.GameRunning()) {
    fNumpad();
  }
}

/*
//------------------------------------------------------------------- Prueba
#include "Button.h"
#define PIN_FLASH 0
Button pbFlash(PIN_FLASH); // Object inicialization


// Millis timing
unsigned long buttonPressTime = 0; // Store the time when the button was last pressed
unsigned long previousMillis = 0; // Store the previous time in milliseconds
const long buttonClearTime = 5000; // Threshold time for clearing the message string, in milliseconds
long interval = 1000; // Interval between readings in milliseconds
pinMode(PIN_FLASH, INPUT); // Set the FLASH pin as an input
  pbFlash.init();

bool pushButtonVal = pushButton.read(); // Read the button value

// Clears the message after a long time
if (pushButtonVal == LOW && millis() - buttonPressTime >= buttonClearTime) {
  message = ""; // Clear the message string
  buttonPressTime = millis(); // Update the button press time
}

// Update the button press time when the button is pressed
if (pushButtonVal == LOW) {
  buttonPressTime = millis();
}
*/