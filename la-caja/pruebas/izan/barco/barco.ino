/*******************************************************
The player has to send the message SOS with morse code.
If the LDR is light up close by a phone light will send a dot.
If not, it will send a dash.

If the player clicks the button it will add the letter of the morse code to the message
To reset the message the player must pulse the button for a longer time.

*! SOS is ...---... 
********************************************************/
int espacioCaracteres = 1000;
int threshold = 100;
/// Variables para almacenar los tiempos de pulso
unsigned long pulsoCorto = 0;
unsigned long pulsoLargo = 0;

// Variable para almacenar el valor del LDR
int ldrValue = 0;

// Variable para almacenar el valor anterior del LDR
int previousLdrValue = 0;

// Variable para almacenar el tiempo actual
unsigned long currentTime = 0;

// Variable para almacenar el tiempo anterior
unsigned long previousTime = 0;

void loop() {
  // Leer el valor del LDR
  ldrValue = analogRead(A0);
  
  // Obtener el tiempo actual
  currentTime = millis();
  
  // Verificar si el valor del LDR cambió
  if (ldrValue != previousLdrValue) {
    // Verificar si el LDR está iluminado o no
    if (ldrValue >= threshold) {
      // Guardar el tiempo de inicio del pulso
      previousTime = currentTime;
    } else {
      // Calcular la duración del pulso
      unsigned long duration = currentTime - previousTime;
      
      // Verificar si el pulso es corto o largo
      if (duration >= pulsoLargo) {
        // Mostrar una raya en la consola serie
        Serial.print("-");
      } else if (duration <= pulsoCorto) {
        // Mostrar un punto en la consola serie
        Serial.print(".");
      }
      
      // Esperar un tiempo entre caracteres
      delay(espacioCaracteres);
    }
    
    // Actualizar el valor anterior del LDR
    previousLdrValue = ldrValue;
  }
}
