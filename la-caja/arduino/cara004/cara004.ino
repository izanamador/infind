/* Librería general  */
#include <infra.h>

/* Libreria convertidor A/D ADS1015 */
#include <Adafruit_ADS1X15.h>

/* Librerías Pantalla Oled SSD1306 */
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

/* Librerías de uso generales*/
#include <array>
#include <string.h>
#include <Wire.h>

/* Librería de cosecha propia*/
#include <Joystick.h>

#define SEND_TIME_ 1

#define x_channel 1
#define y_channel 0
#define SCREEN_WIDTH = 128; // OLED display width, in pixels
#define SCREEN_HEIGHT = 64; // OLED display height, in pixels

/* Initialize the display */
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT);
Joystick joystick(x_channel,y_channel);

String teststr = "rhyloo";

void setup(void){
  Serial.begin(9600);

  joystick.Setup();

  /* Las siguientes lineas son muy importantes, desconozco el motivo, mi idea es
     que si no las pones intenta el bus I2C más rápido de lo que puede y peta */
  /****************************************************************************/
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Display setup failed");
    while (true);
  }
  Serial.println("Display is good to go");
  /****************************************************************************/
}




void loop(void){
  display.clearDisplay();
  joystick.Loop();

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

