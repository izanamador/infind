#ifndef ESPINFIND
#define ESPINFIND
//----- Librerías utilizadas
#include <ESP8266WiFi.h>        
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Button.h>

//--------------------------------------------- ESP
#define ESP_BAUD_RATE         115200

//--------------------------------------------- WIFI
#define WIFI_SSID             "infind"
#define WIFI_PASSWORD         "1518wifi"
#define WIFI_RETRY_DELAY      200

//--------------------------------------------- JSON
#define JSON_MESSAGE_SIZE    512

//----- Pines comunes o estándar
#define PIN_COMUN_LED      2    // REQ.IT3
#define PIN_COMUN_SWITCH   16   // REQ.IT4


class EspInfInd
{ 
  public:
    EspInfInd();
    void setup();
    void loop();
    ~EspInfInd();
    
    // REQ.MQ1 
      char          espId[40];

    // wifi object
      WiFiClient    objWifi;

    // mqtt object
      PubSubClient  *ptrMqtt;
      void          MqttSetup();
      void          MqttConnect();

    //------ REQ.IT29 Interfaz común JSON
      // StaticJsonDocument<JSON_MESSAGE_SIZE> msgPub;
};

#endif

