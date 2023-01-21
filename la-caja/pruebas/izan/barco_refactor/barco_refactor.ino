// Include Libraries
#include <ezButton.h>

// Pin Definitions
#define LDR_PIN A0 // Pin for the LDR sensor
#define PUSHBUTTON_PIN 15
#define PUSHBUTTON_PIN_2 3

#define SENDTIME 5000

// Object inicialization
ezButton buttonSave(PUSHBUTTON_PIN);  
ezButton buttonSend(PUSHBUTTON_PIN_2);  

// String Inicialization
String morseCode = ""; // String to store the Morse code message
String message = "";   // String to store the translated message
String GameAns = "SOS";

void setup()
{
   
    Serial.begin(115200);
  pinMode(LDR_PIN, INPUT); // Set the LDR pin as an input
   buttonSend.setDebounceTime(100);
   buttonSave.setDebounceTime(100);

}

void loop()
{
  buttonSend.loop();
  buttonSave.loop();
  
  if (buttonSend.isReleased())
  {
    Serial.println("Se ha enviado el mensaje 1");
} 
  if (buttonSave.isReleased())
  {
    Serial.println("Se ha enviado guardado");
} 

}

// Function to decode a Morse code character
char decodeMorse(String code)
{
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
  if (code == " ")
  {
    return ' ';
  }

  // Search for the code in the lookup table
  for (int i = 0; i < 26; i++)
  {
    if (code == morseTable[i])
    {
      return (char)('A' + i); // Return the corresponding letter
    }
  }

  // If the code is not found in the lookup table, return a null character
  return '\0';
}
