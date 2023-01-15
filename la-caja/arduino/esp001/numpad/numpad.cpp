// librerías generales
#include <array>
#include <string.h>
#include <Wire.h>
#include <math.h>
#include <cstring>

#include <Keypad.h> // Libreria para usar el numpad 

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
static char strReport[50];


#define MAX_DIGITS 10


void fNumpad() {
  static char strDigits[MAX_DIGITS]; // string para ir guardando el número
  static int iDigit = 0; // posición donde pondré el siguiente caracter

  char key = myKeypad.getKey(); // lectura asíncrona del numpad
  sprintf(strReport, "Ult tecla = %c\n", key);

  if (key == '#') {
    // al pulsar enviar se pone un nulo al final del string para cortar la cadena
    strDigits[iDigit] = '\0'; 
    sprintf(strReport, "Respuesta=%s, solucion=%s\n", strDigits, oNumpad.gameparam);    
    iDigit = 0; // restablecer la posición para el nuevo intento o siguiente partida

    if (strcmp(strDigits, oNumpad.gameparam)==0) {
      oNumpad.ReportSuccess(strReport);
    } else {
      oNumpad.ReportFail(strReport);
    }        
  }
  else if (key >= '0' && key <= '9') {
    if (iDigit <= MAX_DIGITS) {
      strDigits[iDigit++] = key;
    }
    oNumpad.ReportStatus(strDigits);
  }
}