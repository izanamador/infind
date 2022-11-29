/*
  Morse.cpp - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/

/*
First, make a Morse directory inside of the libraries sub-directory of your sketchbook directory. 
Copy or move the Morse.h and Morse.cpp files into that directory. 
Now launch the Arduino environment. If you open the Sketch > Import Library menu, you should see Morse inside. 
The library will be compiled with sketches that use it. 
If the library doesn't seem to build, make sure that the files really end in .cpp and .h 
(with no extra .pde or .txt extension, for example).

*/
#include "Arduino.h"
#include "Cubo.h"

#define APAGADO HIGH
#define ENCENDIDO LOW

Cubo::Cubo(int pin)
{
  pinMode(pin, OUTPUT);
  _pin = pin;
}


void Cubo::dot()
{
  digitalWrite(_pin, ENCENDIDO);
  delay(250);
  digitalWrite(_pin, APAGADO);
  delay(250);  
}


void Cubo::dash()
{
  digitalWrite(_pin, ENCENDIDO);
  delay(1000);
  digitalWrite(_pin, APAGADO);
  delay(250);
}

void Cubo::wait()
{
  digitalWrite(_pin, APAGADO);
  delay(5000);
}