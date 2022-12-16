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

#ifndef Joystick_
#define Joystick_
#define CALIBRATION_VALUE 1000
#define DIFFERENCE_ERROR_VALUE 800

class Joystick: public Adafruit_ADS1015{
private:

    struct datos{
        char channel = 0;
        short value = 0;
        int value_read = 0;
        int value_fixed = 0;
        int  offset = 0;
    };

    struct coords{
        datos x;
        datos y;
    } coordenadas;

    float angle;
    char direction = 0;

public:
    Joystick(char x_channel, char y_channel);
    void Loop();
    void Setup();
    short GetX();
    short GetY();
    void Calibration();
    ~Joystick();



};

#endif
