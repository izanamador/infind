#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <math.h>
#include <array>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <string.h>
#define SEND_TIME_ 1
const float DEG2RAD = PI / 180.0f;
const float RAD2DEG = 180.0f / PI;
const int SCREEN_WIDTH = 128; // OLED display width, in pixels
const int SCREEN_HEIGHT = 64; // OLED display height, in pixels
 

 

// Crear objeto de la clase
//Adafruit_ADS1115 ads;         /* Use this for the 16-bit version */
Adafruit_ADS1015 ads;     /* Use this for the 12-bit version */

// initialize the display:
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT);

int offsets[2] = {0, 0};

void Calibration(short input, int channel){
  for (int i = 0; i <= 1000; i++) {
     if(input-(offsets[channel]+i) == 0){
      offsets[channel] = offsets[channel] + i;
     }else if(input-(offsets[channel]-i) == 0){
      offsets[channel] = offsets[channel] - i;
     }
    }
}

String teststr = "rhyloo";

void Direction(int direction){
  static int actual_direction = 0;
    if(direction != actual_direction){
    actual_direction = direction;
    switch(direction) {
    case 0:
      teststr = "Centro";
      Serial.println("Centro");
      break;
    case 1:
      teststr = "Derecha";
      Serial.println("Derecha");
      break;
    case 2:
      teststr = "Arriba";
      Serial.println("Arriba");
      break;
    case 3:
      teststr = "Izquierda";
      Serial.println("Izquierda");
      break;
    case 4:
      teststr = "Abajo";
      Serial.println("Abajo");
      break;
    }
  }
}

void setup(void) 
{
  Serial.begin(9600); 
  // Iniciar el ADS1115
  ads.begin();
  short x = ads.readADC_SingleEnded(1);
  Calibration(x,1);
  short y = ads.readADC_SingleEnded(0);
  Calibration(y,0);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Display setup failed");
    while (true);
  }
  Serial.println("Display is good to go");
}



void loop(void)
{
  display.clearDisplay();
  int joystick_direction;  
  float angle = 0.0;
  static int x_read = 850;
  static int y_read = 0;

  int x = ads.readADC_SingleEnded(1);
  int y = ads.readADC_SingleEnded(0);

  int x_fixed = x - offsets[0];
  int y_fixed = y - offsets[1];

  if(abs(x_read-x_fixed) > 800){
    x_read = x_fixed;
  }

  if(abs(y_read-y_fixed) > 800){
    y_read = y_fixed;
  }

  if(abs(x_fixed) < 800){
    x_fixed = 0;
  }

  if(abs(y_fixed) < 800){
    y_fixed = 0;
  }

  //Serial.print("X: ");Serial.println(x);
  //Serial.print("Y: ");Serial.println(y);
  //Serial.print("X_fixed: ");Serial.println(x_fixed);
  //Serial.print("Y_fixed: ");Serial.println(y_fixed);
  //Serial.print("X_read: ");Serial.println(x_read);
  //Serial.print("Y_read: ");Serial.println(y_read);

  angle =  atan2(y_read,x_read);

  //Serial.print("Ángulo: ");Serial.println(angle);
    // ¿Está en el centro?
  if(x_fixed == 0 && y_fixed == 0){
    joystick_direction = 0;
  }else if (angle >= 0 ){
    if (angle <= 45*DEG2RAD) {
      joystick_direction = 1; //derecha
    } else if (angle >= (90+45)*DEG2RAD){
      joystick_direction = 3; //izquierda
    }else{
      joystick_direction = 2;} //arriba
  }else{
    if (angle >= -45*DEG2RAD){
      joystick_direction = 1; //derecha
    }else if (angle <= (-90-45)*DEG2RAD){
      joystick_direction = 3; //izquierda
    }else{
      joystick_direction = 4;} //abajo
  }
  
  
  Direction(joystick_direction);
  // static String teststr = "rhyloo";
   if (Serial.available() > 0) {
  teststr = Serial.readString();  //read until timeout
  teststr.trim();                        // remove any \r \n whitespace at the end of the String
  }
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.print(teststr);
  display.display();
}

// // constants won't change. They're used here to set pin numbers:
// const int ledPin = 13;
// const int pinLED = 13;
// const int pinJoyX = A0;
// const int pinJoyY = A1;
// const int pinJoyButton = 9;

// // Variables will change:
// int ledState = HIGH;        // the current state of the output pin
// int buttonState;            // the current reading from the input pin
// int lastButtonState = LOW;  // the previous reading from the input pin

// // the following variables are unsigned longs because the time, measured in
// // milliseconds, will quickly become a bigger number than can be stored in an int.
// unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
// unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

// void setup() {
//   pinMode(ledPin, OUTPUT);
//   digitalWrite(ledPin, ledState);

//   pinMode(pinJoyButton , INPUT_PULLUP);   //activar resistencia pull up
//   Serial.begin(115200);
// }

// void loop() {
//   static unsigned long ultimo_mensaje = 0;
//   unsigned long ahora = millis();
//   int Xvalue = 0;
//   int Yvalue = 0;
//   bool buttonValue = false;
  
//   //leer valores
//   if (ahora - ultimo_mensaje >= SEND_TIME_) {
//     Xvalue = analogRead(pinJoyX);
//     ultimo_mensaje = ahora;
//   }
//   Yvalue = analogRead(pinJoyY);
//   buttonValue = digitalRead(pinJoyButton);

//   //mostrar valores por serial
//   Serial.print("X:" );
//   Serial.print(Xvalue);
//   Serial.print(" | Y: ");
//   Serial.print(Yvalue);
//   Serial.print(" | Pulsador: ");
//   Serial.println(buttonValue);

//     // read the state of the switch into a local variable:
//   int reading = digitalRead(pinJoyButton);

//   // check to see if you just pressed the button
//   // (i.e. the input went from LOW to HIGH), and you've waited long enough
//   // since the last press to ignore any noise:

//   // If the switch changed, due to noise or pressing:
//   if (reading != lastButtonState) {
//     // reset the debouncing timer
//     lastDebounceTime = millis();
//   }

//   if ((millis() - lastDebounceTime) > debounceDelay) {
//     // whatever the reading is at, it's been there for longer than the debounce
//     // delay, so take it as the actual current state:

//     // if the button state has changed:
//     if (reading != buttonState) {
//       buttonState = reading;

//       // only toggle the LED if the new button state is HIGH
//       if (buttonState == HIGH) {
//         ledState = !ledState;
//       }
//     }
//   }

//   // set the LED:
//   digitalWrite(ledPin, reading);

//   // save the reading. Next time through the loop, it'll be the lastButtonState:
//   lastButtonState = reading;
// }
