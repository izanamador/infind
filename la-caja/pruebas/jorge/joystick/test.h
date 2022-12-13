/*

  Clase Infra
    Implementa la infraestructura técnica común a todos los sketchs de la-caja
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

#ifndef test_
#define test_
#include "Arduino.h"

class test
{
  public:
    test();
    int Setup(void (*mqttCallback)(char*, byte*, unsigned int));
    int Loop();
    ~test();

  private:
      void          PrintConfig();

};

#endif
