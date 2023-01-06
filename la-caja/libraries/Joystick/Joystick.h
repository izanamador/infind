/*********************************************************************************************************/
/* Clase Joystick                                                                                        */
/* Implementa una serie de funciones específicas del dispositivo joystick.                               */
/* Librerías requeridas                                                                                  */
/*   - Arduino - Funciones propias de arduino                                                            */
/*   - Math - Librería para utilizar funciones matemáticas                                               */
/*   - Adafruit ADS1x15 - Librería para usar un módulo I2C que convierte señales analógicas en digitales */
/*                                                                                                       */
/* Enlaces:                                                                                              */
/* Añadir proximamente                                                                                   */
/* Constantes:                                                                                           */
/*   - CALIBRATION_VALUE: Valor que permite calibrar el joystick cuando lo centramos en el (0,0)         */
/*   - DIFFERENCE_ERROR_VALUE: Permite diferencia un cambio de valor entre el actual y siguiente         */
/*********************************************************************************************************/

#include <Adafruit_ADS1X15.h>
#include "Arduino.h"
#include <math.h>

#ifndef Joystick_
#define Joystick_

#define CALIBRATION_VALUE 1000
#define DIFFERENCE_ERROR_VALUE 750
#define DEBOUNCE_TIME 300

#define DEGREES_45 0.785398
#define DEGREES_135 2.35619
// #define CALIBRATION_TIME 600000

#define DERECHA 1
#define ARRIBA 2
#define IZQUIERDA 3
#define ABAJO 4

class Joystick: public Adafruit_ADS1015{
private:

  struct datos{
    char channel = 0;

    short joystickValue;
    short previousJoystickValue = 0;
    int joystickValueRead = 0;
    int joystickValueFixed = 0;
    int joystickValueOffset = 0;
  };

  struct coords{
    datos x;
    datos y;
  } coordenada;

  float angle;
  char direction;
  char lastdirection;
  unsigned long startTime;


public:
  Joystick(char x_channel, char y_channel);
  void Setup();
  void Calibration();
  void Direction();
  int Loop();
  ~Joystick();
};

#endif
