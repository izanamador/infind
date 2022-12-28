 /*******************************************************
Enter the next value of a discrete equation sequence of an 
IQ test in binary by means of switches and a button.
The possible number to represent will be 2^n-1, 
thus having for n = 5 switches, the representation 
of the range [0-31].
The button will be used to send the correct answer.

This version is only an example with the following problem:
Secuence: 3 5 7 11 13 
Correct number: 17
********************************************************/
// Include Libraries
#include "Arduino.h"
#include "Button.h"

// Pin Definitions
#define PUSHBUTTON_PIN_2  5
#define TOGGLESWITCH_1_PIN_2  4
#define TOGGLESWITCH_2_PIN_2  10
#define TOGGLESWITCH_3_PIN_2  2
#define TOGGLESWITCH_4_PIN_2  14
#define TOGGLESWITCH_5_PIN_2  12

// Global variables and defines

// object initialization
Button pushButton(PUSHBUTTON_PIN_2);
Button ToggleSwitch_1(TOGGLESWITCH_1_PIN_2);
Button ToggleSwitch_2(TOGGLESWITCH_2_PIN_2);
Button ToggleSwitch_3(TOGGLESWITCH_3_PIN_2);
Button ToggleSwitch_4(TOGGLESWITCH_4_PIN_2);
Button ToggleSwitch_5(TOGGLESWITCH_5_PIN_2);

//define vars for program
int COMBINACION_CORRECTA_001 = 17;

void setup() 
{
    Serial.begin(9600);
    while (!Serial) ; // wait for serial port to connect. Needed for native USB
    Serial.println("start");
   
    pushButton.init();
    ToggleSwitch_1.init();
    ToggleSwitch_2.init();
    ToggleSwitch_3.init();
    ToggleSwitch_4.init();
    ToggleSwitch_5.init();
}

void loop() 
{
  std::array<int, 5> COMBINACION_ACTUAL_ARRAY {
    ToggleSwitch_1.read(),
    ToggleSwitch_2.read(),
    ToggleSwitch_3.read(),
    ToggleSwitch_4.read(),
    ToggleSwitch_5.read()};
    
  int COMBINACION_ACTUAL = 0;
  
  for (int i = 0; i < 5; i++) {
    COMBINACION_ACTUAL += COMBINACION_ACTUAL_ARRAY[i] << i;
  }

  Serial.println(COMBINACION_ACTUAL);
  
 if(COMBINACION_ACTUAL == COMBINACION_CORRECTA_001)
      { 
      Serial.println(F("¡Ganaste! Felicidades Tienes 150 de IQ"));
      }
}
