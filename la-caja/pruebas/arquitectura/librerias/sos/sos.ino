// https://docs.arduino.cc/learn/contributions/arduino-creating-library-guide

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