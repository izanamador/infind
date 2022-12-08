/*
  Enlaces:
    - https://www.arduino.cc/en/Main/Products
    - https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
    - http://librarymanager/All#DHTesp

  Constantes
    - LED_BUILTIN is set to the correct LED pin independent of which board is used.

  Librerías requeridas
    - DHT sensor library for ESPx
    - PubSub-Cient by Nick O'Leary
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define MQTT_MAX_TOPIC   128
#define MQTT_MAX_SUBSCRIPTIONS 5
#define MQTT_MAX_MESSAGE 128

class Infra
{
  public:
    Infra();
    int Setup(void (*callback)(char*, byte*, unsigend int));
    int Loop();
    ~Infra();
    
    // esp board
      char          espId[40];

    // wifi object
      WiFiClient    objWifi;

    // mqtt object
      PubSubClient  objMqtt(objWifi);
      void          MqttSetup();
      void          MqttConnect();
      void          MqttPublish(char *message);
      char          mqttTopicPub[MQTT_MAX_TOPIC];
      char          mqttTopicsSub[MQTT_MAX_SUBSCRIPTIONS][MQTT_MAX_TOPIC];
    
  private:
    
    char _wifiSsid[40];
    char _wifiPassword[40];

    char _mqttServer[40];
    char _mqttUser[40];
    char _mqttPassword[40];
};
