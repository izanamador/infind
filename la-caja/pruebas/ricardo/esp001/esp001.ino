 //#include "EspInfInd.h"
 EspInfInd objEsp;


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    delay(1000);
    objEsp.setup();
    delay(2000);
}

// the loop function runs over and over again forever
void loop() {
  static unsigned int msLast=0;
  objEsp.loop();
  if (msLast+60000<millis()) {
    Serial.printf("Sigo vivo");
    msLast=millis();
  }
}
