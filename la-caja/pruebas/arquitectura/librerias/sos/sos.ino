// https://docs.arduino.cc/learn/contributions/arduino-creating-library-guide
#include <cubo.h>
Cubo cubo(2); // crea el objeto e inicializa el pin 2
void setup()
{

}

void loop()
{
  cubo.dot(); cubo.dot(); cubo.dot();
  cubo.dash(); cubo.dash(); cubo.dash();
  cubo.dot(); cubo.dot(); cubo.dot();
  cubo.wait();
}

/*

int pin = 2; // 13;


void setup()

{

  pinMode(pin, OUTPUT);

}


void loop()

{

  dot(); dot(); dot();

  dash(); dash(); dash();

  dot(); dot(); dot();

  delay(3000);

}


void dot()

{

  digitalWrite(pin, HIGH);

  delay(250);

  digitalWrite(pin, LOW);

  delay(250);

}


void dash()

{

  digitalWrite(pin, HIGH);

  delay(1000);

  digitalWrite(pin, LOW);

  delay(250);

}
*/