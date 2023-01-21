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
/* #define OTA_HTTP_ADDRESS      F("iot.ac.uma.es")         // Address of OTA update server */
#define OTA_HTTP_PATH         F("/lacaja/update") // Path to update firmware
#define OTA_HTTP_PORT         1880                     // Port of update server                                                       // Name of firmware
#define OTA_HTTP_VERSION      String(__FILE__).substring(String(__FILE__).lastIndexOf('\\')+1) + ".nodemcu" 

//--------------------------------------------- MQTT
#include <PubSubClient.h>
#define MQTT_SERVER           "iot.ac.uma.es"
#define MQTT_PORT             1883
#define MQTT_BUFFER_SIZE      512
#define MQTT_USER             "II3"
#define MQTT_PASSWORD         "qW30SImD"
#define MQTT_RETRY_DELAY      5000
#define MQTT_LASTWILL         "{\"online\":false}"
#define MQTT_CONNECT_MSG      "{\"online\":true}"

#define TOPIC_MAIN            0 // publicacion y lastwill
//#define TOPIC_NUM_CFG         1 // configuración
#define TOPIC_NUM_CMD         1 // ejecución de comandos
#define TOPIC_NUM_MAX         2


//--------------------------------------------- JSON
#include <ArduinoJson.h>
#include <EEPROM.h>
#define JSON_MESSAGE_SIZE    512

//---------------------------------------------- GAME STATUS

class Infra
{
public:
  Infra();
  int Setup(void (*mqttCallback)(char*, byte*, unsigned int));
  /* int Loop(char* GameInfo); */
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
  char          *mqttTopicsPub[TOPIC_NUM_MAX];
  char          *mqttTopicsSub[TOPIC_NUM_MAX];
  int           GameRunning();
  void          ReportStatus(char* GameInfo);
  void          ReportStart(char* GameInfo);
  void          ReportFailure(char* GameInfo);
  void          ReportSuccess(char* GameInfo);
  void          RestartBoard();
  void          setOTAAddress(const char* Address);
  // JSON
  StaticJsonDocument<JSON_MESSAGE_SIZE> objConfig;
private:
  void          PrintConfig();
  int           currStat_;
  //int           lastStat_;
  unsigned int  milLsRep_;
  unsigned int  milStart_;
  unsigned int  milTries_;
  int           numTries_;
  char* OTA_HTTP_ADDRESS2;
  char* global_game_info;
      
};

#endif


