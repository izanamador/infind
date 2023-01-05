/////////////////////////////////////////////////////////////////
// https://github.com/LennartHennigs/Button2
#include "Button2.h"

/////////////////////////////////////////////////////////////////

#define BUTTON_PIN  15

/////////////////////////////////////////////////////////////////

Button2 button;

/////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  delay(50);
  Serial.println("\n\nButton Demo");
  
  button.begin(BUTTON_PIN);
  button.setClickHandler(singleClick);
  button.setLongClickHandler(longClick);
  button.setDoubleClickHandler(doubleClick);
  button.setTripleClickHandler(tripleClick);
}

/////////////////////////////////////////////////////////////////

void loop() {
  button.loop();
}

/////////////////////////////////////////////////////////////////

void singleClick(Button2& btn) {
    Serial.println("click\n");
}
void longClickDetected(Button2& btn) {
    Serial.println("long click detected");
}
void longClick(Button2& btn) {
    Serial.println("long click\n");
}
void doubleClick(Button2& btn) {
    Serial.println("double click\n");
}
void tripleClick(Button2& btn) {
    Serial.println("triple click\n");
}
/////////////////////////////////////////////////////////////////
