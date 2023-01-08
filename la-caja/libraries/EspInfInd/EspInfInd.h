#ifndef ESPINFIND
#define ESPINFIND

//--------------------------------------------- ESP
#define ESP_BAUD_RATE         115200

//--------------------------------------------- WIFI
#include <ESP8266WiFi.h>        
#define WIFI_SSID             "infind"
#define WIFI_PASSWORD         "1518wifi"
#define WIFI_RETRY_DELAY      200

//--------------------------------------------- MQTT
#include <PubSubClient.h>

//strTopicSubGameCmd        char[50];
//strTopicSubGameEst        char[50];
//strTopicSubFmtConfig      char[50];
//strTopicSubLedCmd         char[50];
//strTopicSubSwithcCmd      char[50];
//strTopicSubFota           char[50];
//strTopicPubFmtDatos       char[50];
//strTopicPubLedStatus      char[50];
//strTopicPubEspSwiStatus   char[50];
//strTopicSubAllFmtCfg   char[50];
//strTopicSubAllLedCmd      char[50];
//strTopicSubAllSwiCmd   char[50];
//strTopicSubAllFota        char[50];



//--------------------------------------------- JSON
#include <ArduinoJson.h>
#define JSON_MESSAGE_SIZE    512

//---------------------------------------------- GAME STATUS

class EspInfInd
{
  public:
    EspInfInd();
    int Setup();
    void Loop();
    ~EspInfInd();
    
    // REQ.MQ1 
      char          espId[40];

    // wifi object
      WiFiClient    objWifi;

    // mqtt object
      PubSubClient  *ptrMqtt;
      void          MqttSetup();
      void          MqttConnect();
      void          MqttPublish(char *message);

  private:
    char strTopicPubConex_[100];
    char strTopicSubConf_ [100];
      
};

#endif


/*

#define ESP_LED_OTA           16

//--------------------------------------------- OTA
#include <ESP8266httpUpdate.h>
#define OTA_HTTP_ADDRESS      F("iot.ac.uma.es")         // Address of OTA update server
#define OTA_HTTP_PATH         F("/esp8266-ota/update") // Path to update firmware
#define OTA_HTTP_PORT         1880                     // Port of update server                                                       // Name of firmware
#define OTA_HTTP_VERSION      String(__FILE__).substring(String(__FILE__).lastIndexOf('\\')+1) + ".nodemcu" 


#define TOPIC_MAIN            0 // publicacion y lastwill
#define TOPIC_NUM_CFG         1 // configuración
#define TOPIC_NUM_CMD         2 // ejecución de comandos
#define TOPIC_NUM_MAX         5


      int           GameRunning();
      void          ReportStatus(char* GameInfo);
      void          ReportStart(char* GameInfo);
      void          ReportFailure(char* GameInfo);
      void          ReportSuccess(char* GameInfo);
    
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

      char          *mqttTopicsPub[TOPIC_NUM_MAX];
      char          *mqttTopicsSub[TOPIC_NUM_MAX];

*/

