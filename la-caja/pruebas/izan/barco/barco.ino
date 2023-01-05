// Include Libraries
#include "Arduino.h"
#include "Button.h"

// Pin Definitions
#define LDR_PIN A0 // Pin for the LDR sensor
#define PUSHBUTTON_PIN_2  15

Button pushButton(PUSHBUTTON_PIN_2);

String morseCode = ""; // String to store the Morse code message
String message = ""; // String to store the translated message

void setup() {
  Serial.begin(9600); // Initialize serial communication
  pinMode(LDR_PIN, INPUT); // Set the LDR pin as an input
  pushButton.init();
}

void loop() {
  int ldrValue = analogRead(LDR_PIN); // Read the value of the LDR sensor
  bool pushButtonVal = pushButton.read();
  
  Serial.println(ldrValue); // CALIBRATION
  
  if (ldrValue < 100) { // If the LDR value is above a certain threshold, it means the light is on
    morseCode += "."; // Add a "dot" to the Morse code message
    delay(1000); // Wait for 100 milliseconds
  } else {
    morseCode += "-"; // Add a "dash" to the Morse code message
    delay(3000); // Wait for 300 milliseconds
  }
  
  Serial.println(morseCode); // CALIBRATION
    
  // Check if a complete Morse code character has been received
  if (pushButtonVal == HIGH) { // A space indicates the end of a character
    char c = decodeMorse(morseCode); // Decode the character
    if (c != '\0') { // If the character is valid, add it to the message
      message += c;
    }
    morseCode = ""; // Clear the Morse code string for the next character
  }

  Serial.println(message); // Print the message to the serial monitor
}


// Function to decode a Morse code character
char decodeMorse(String code) {
  // Morse code lookup table
  const char morseTable[26][5] = {
    ".-",   // A
    "-...", // B
    "-.-.", // C
    "-..",  // D
    ".",    // E
    "..-.", // F
    "--.",  // G
    "....", // H
    "..",   // I
    ".---", // J
    "-.-",  // K
    ".-..", // L
    "--",   // M
    "-.",   // N
    "---",  // O
    ".--.", // P
    "--.-", // Q
    ".-.",  // R
    "...",  // S
    "-",    // T
    "..-",  // U
    "...-", // V
    ".--",  // W
    "-..-", // X
    "-.--", // Y
    "--.."  // Z
  };
  
  // Check if the code is a space
  if (code == " ") {
    return ' ';
  }
  
  // Search for the code in the lookup table
  for (int i = 0; i < 26; i++) {
    if (code == morseTable[i]) {
      return (char)('A' + i); // Return the corresponding letter
    }
  }
  
  // If the code is not found in the lookup table, return a null character
  return '\0';
}
