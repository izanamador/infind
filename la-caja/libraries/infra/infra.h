/*

  Clase Infra
    Implementa la infraestructura técnica común a todos los sketchs de la-caja
    - Coneión a Wifi
    - Conexión a MQTT
    - Tratamiento de datos JSON
    - OTA

  Enlaces:
    - https://www.arduino.cc/en/Main/Products
    - https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
    - http://librarymanager/All#DHTesp
    - https://arduinojson.org/v6/doc/

  Constantes
    - LED_BUILTIN is set to the correct LED pin independent of which board is used.

  Librerías requeridas
    - DHT sensor library for ESPx
    - PubSub-Cient by Nick O'Leary
    - ArduinoJson by Benoit Blanchon 6.19.4

*/

#ifndef INFRAH_
#define INFRAH_

//--------------------------------------------- ESP
#define ESP_BAUD_RATE         115200
#define ESP_LED_OTA           16

//--------------------------------------------- WIFI
#include <ESP8266WiFi.h>        
#define WIFI_SSID             "infind"
#define WIFI_PASSWORD         "1518wifi"
#define WIFI_RETRY_DELAY      200

//--------------------------------------------- OTA
#include <ESP8266httpUpdate.h>
#define OTA_HTTP_ADDRESS      F("172.16.53.###")         // Address of OTA update server
#define OTA_HTTP_PATH         F("/esp8266-ota/update") // Path to update firmware
#define OTA_HTTP_PORT         1880                     // Port of update server                                                       // Name of firmware
#define OTA_HTTP_VERSION      String(__FILE__).substring(String(__FILE__).lastIndexOf('\\')+1) + ".nodemcu" 

//--------------------------------------------- MQTT
#include <PubSubClient.h>
#define MQTT_SERVER           "iot.ac.uma.es"
#define MQTT_PORT             1883
#define MQTT_BUFFER_SIZE      512
#define MQTT_USER             "infind"
#define MQTT_PASSWORD         "zancudo"
#define MQTT_RETRY_DELAY      5000
#define MQTT_LASTWILL         "{\"online\":false}"
#define MQTT_CONNECT_MSG      "{\"online\":true}"

#define TOPIC_NUM_CFG         0 // configuración
#define TOPIC_NUM_CMD         1 // ejecución de comandos
#define TOPIC_NUM_MAX         5


//--------------------------------------------- JSON
#include <ArduinoJson.h>
#define JSON_MESSAGE_SIZE    512



class Infra
{
  public:
    Infra();
    int Setup(void (*mqttCallback)(char*, byte*, unsigned int));
    int Loop();
    ~Infra();
    
    // esp board
      char          espId[40];

    // wifi object
      WiFiClient    objWifi;

    // mqtt object
      PubSubClient  *ptrMqtt;;
      void          MqttSetup();
      void          MqttConnect();
      void          MqttPublish(char *message);
      char          *mqttTopicPub;
      char          *mqttTopicsSub[TOPIC_NUM_MAX];
    
    // JSON
      StaticJsonDocument<JSON_MESSAGE_SIZE> objConfig;
  private:
    
};

#endif