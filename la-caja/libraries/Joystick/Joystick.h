/*

  Clase Joystick
  Implementa una serie de funciones específicas del dispositivo joystick.
  - Coneión a Wifi
  - Conexión a MQTT
  - Tratamiento de datos JSON
  - OTA

  Config IDE:
  - File > Preferences > Additional Boards Manager:
  http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Tools > Board > Boards Manager:
  esp8266 by ESP8266 Community
  - Tools > Board > ESP8266:
  NodeMCU 1.0 ESP-12E Module

  Librerías requeridas
  - DHT sensor library for ESPx
  - PubSub-Cient by Nick O'Leary
  - ArduinoJson by Benoit Blanchon 6.19.4

  Enlaces:
  - https://www.arduino.cc/en/Main/Products
  - https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
  - http://librarymanager/All#DHTesp
  - https://arduinojson.org/v6/doc/


  Constantes
  - LED_BUILTIN is set to the correct LED pin independent of which board is used.


*/

/* Libreria convertidor A/D ADS1015 */
#include <Adafruit_ADS1X15.h>

#ifndef Joystick_
#define Joystick_

#define MAX_VALUE 1000
#define CALIBRATION_VALUE 800

class Joystick: public Adafruit_ADS1015{
private:

    struct datos{
        char channel = 0;
        short value = 0;
        int value_read = 0;
        int value_fixed = 0;
        int  offset = 0;
    };

    struct coords{
        datos x;
        datos y;
    } coordenadas;

    float angle;
    char direction = 0;
    Adafruit_ADS1015 adss;

public:
    Joystick(char x_channel, char y_channel);

    // void deg2rad();
    // void rad2deg();
    // void AvailableSerial();

    void Loop();
    void Setup();
    short GetX();
    short GetY();
    void Calibration();
    ~Joystick();



};

#endif
