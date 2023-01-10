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

      
      bool bStandard_; // si la configuración de pines se ajusta a la especificación
      char strBoardName_[20]; // nombre simbólico de la placa

      //-- Estados del chip
      long stSwLevel_ = 0;    // Nivel inicial del switch = 0 encendido
      long stLdLevel_ = 0;    // Nivel inicial del led = 0 encendido
      unsigned int stLedBrig_ = 0;    // milisegundos desde orden de encender led
      unsigned int stPerStat_ = 0;    // milisegundos desde último estado = 0
      unsigned int stPerFota_ = 0;    // milisegundos desde última actualización fota

      //--------------------- configuración
      int cfPerStat_ = 30;   // frecuencia de envío de mensajes de estado (segundos)
      int cfPerFota_ = 0;    // periodo de actualizaciones FOTA (minutos)
      int cfSwLight_ = 0;    // configuración de qué valor representa luz encendida 0 o 1
      int cfLdLight_ = 0;    //  "  pero para el led
      int cfLedBrig_ = 50;   // brillo hasta el que tiene que llegar cuando se enciende (50%)
      int cfLedVelo_ = 1;    // velocidad de cambio del brillo (1% cada 10 ms)


};

#endif

