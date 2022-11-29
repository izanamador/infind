/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/


#include "Arduino.h"
#ifndef Cubo_h
#define Cubo_h



class Cubo
{

  public:

    Cubo(int pin);
    void dot();
    void dash();

  private:
    int _pin;
};



#endif
