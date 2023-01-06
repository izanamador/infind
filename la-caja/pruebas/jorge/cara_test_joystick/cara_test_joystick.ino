#include <Arduino.h>
#include <Joystick.h>

#define x_channel 1
#define y_channel 0

Joystick joystick(x_channel,y_channel);

void setup(void) 
{
  Serial.begin(9600);
  
  joystick.Setup();
}



void loop(void)
{
  int value = joystick.Loop();
  if (value != -1){
      Serial.println(value);
  }
  
}
