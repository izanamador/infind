/*******************************************************
The player has to send the message SOS with morse code.
If the LDR is light up close by a phone light will send a dot.
If not, it will send a dash.

If the player clicks the button it will add the letter of the morse code to the message
To reset the message the player must pulse the button for a longer time.

*! SOS is ...---... 
********************************************************/
// Include Libraries
#include "Arduino.h"
#include "Button.h"

// Pin Definitions
#define LDR_PIN A0 // Pin for the LDR sensor
#define PUSHBUTTON_PIN_2 15

// Millis timing
unsigned long buttonPressTime = 0; // Store the time when the button was last pressed
unsigned long previousMillis = 0;  // Store the previous time in milliseconds
const long buttonClearTime = 5000; // Threshold time for clearing the message string, in milliseconds
long interval = 1000;              // Interval between readings in milliseconds

// Object inicialization
Button pushButton(PUSHBUTTON_PIN_2);

// String Inicialization
String morseCode = ""; // String to store the Morse code message
String message = "";   // String to store the translated message
String GameAns = "SOS";

void setup()
{
  Serial.begin(9600);      // Initialize serial communication
  pinMode(LDR_PIN, INPUT); // Set the LDR pin as an input
  pushButton.init();
}

void loop()
{

  bool pushButtonVal = pushButton.read(); // Read the button value

  // Clears the message after a long time
  if (pushButtonVal == LOW && millis() - buttonPressTime >= buttonClearTime)
  {
    message = "";               // Clear the message string
    buttonPressTime = millis(); // Update the button press time
  }

  // Update the button press time when the button is pressed
  if (pushButtonVal == LOW)
  {
    buttonPressTime = millis();
  }

  int ldrValue = analogRead(LDR_PIN); // Read the value of the LDR sensor
                                      // Serial.println(ldrValue); // CALIBRATION

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    int ldrValue = analogRead(LDR_PIN); // Read the value of the LDR sensor

    if (ldrValue < 100)
    {                   // If the LDR value is above a certain threshold, it means the light is on
      morseCode += "."; // Add a "dot" to the Morse code message
      interval = 3000;  // Set the interval to 1 second for the next reading
    }
    else
    {
      morseCode += "-"; // Add a "dash" to the Morse code message
      interval = 6000;  // Set the interval to 3 seconds for the next reading
    }
  }

  Serial.println(morseCode); // CALIBRATION

  // Check if a complete Morse code character has been received
  if (pushButtonVal == HIGH)
  {                                  // A space indicates the end of a character
    char c = decodeMorse(morseCode); // Decode the character
    if (c != '\0')
    { // If the character is valid, add it to the message
      message += c;
      if (GameAns == message)
      {
        Serial.println("YOU WON");
      }
    }
    morseCode = ""; // Clear the Morse code string for the next character
  }
  Serial.println(message); // Print the message to the serial monitor
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
