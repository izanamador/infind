#ifndef ESPINFIND
#define ESPINFIND


//------ Librerías utilizadas
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

//------ Orígenes de mensajes desde ESP a NodeRED
#define STR_ORG_BOARD  "board"
#define STR_ORG_MQTT   "mqtt"

class EspInfInd
{ 
  public:
    EspInfInd(const char *strBoardName, bool bStandard);
    void Setup(void (*mqttCallback)(char*, byte*, unsigned int));
    void Loop();
    void MqttReceived(char* strTopic, byte* payload, unsigned int length);
    void MqttSend(char* strTopic, char* strGameStatus, const char *strSrc=STR_ORG_BOARD);
    void UpdateSwitch(int iUpdateType, long newLevel, long newConfig);
    ~EspInfInd();
    
    // REQ.MQ1 
      char          espId[40];

    // wifi object
      WiFiClient    objWifi;

    // json 
      StaticJsonDocument<JSON_MESSAGE_SIZE> jsonPub;
      StaticJsonDocument<JSON_MESSAGE_SIZE> jsonSub;

    // mqtt object
      PubSubClient  *ptrMqtt;
      void          MqttSetup();
      void          MqttConnect();

    //------ REQ.IT29 Interfaz común JSON
      // StaticJsonDocument<JSON_MESSAGE_SIZE> msgPub;
private:
    //------ Nomenclatura de topics
      //---- Topics de publicación
      char strTopicPubConex_[100];
      char strTopicPubDatos_[100];
      char strTopicPubStLed_[100];
      char strTopicPubStSwi_[100];
       //--- Topics de suscripción individual
      char strTopicSubConfig_[100];
      char strTopicSubCmdLed_[100];
      char strTopicSubCmdSwi_[100];
      char strTopicSubCmdOta_[100];
       //--- Topics de suscripción grupal
      char strTopicAllConfig_[100];
      char strTopicAllCmdLed_[100];
      char strTopicAllCmdSwi_[100];
      char strTopicAllCmdOta_[100];
       //--- Topics de suscripción grupal
      char strTopicPubJuegos_[100];
      char strTopicAllJuegos_[100];

      long stSwitchLev_;
      long cfSwitchOn_;
      bool bStandard_; // si la configuración de pines se ajusta a la especificación


      char strBoardName_[20]; // nombre simbólico de la placa

};

#endif

