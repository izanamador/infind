#ifndef INFRACPP_
#define INFRACPP_

#include "Arduino.h"
#include "infra.h"

Infra::Infra()
{
  // hardcodes relacionados con la placa
    sprintf(espId, "ESP_%d", ESP.getChipId());

  // hardcodes relacionados con el wifi
    sprintf(_wifiSsid, "infind");
    sprintf(_wifiPassword, "1518wifi");
    
  // hardcodes relacionados con MQTT  
    sprintf(_mqttServer, "iot.ac.uma.es");
    sprintf(_mqttUser,"infind");
    sprintf(_mqttPassword,"zancudo");

    ptrMqtt = new PubSubClient(objWifi);
      
}

int Setup(char *topicPub, char **topicsSub, void (*callback)(char*, byte*, unsigned int));


int Infra::Setup(void (*mqttCallback)(char*, byte*, unsigned int))
{
  //---------------------------------------------- ESP Setup
  Serial.begin(115200);
  Serial.println();
  Serial.printf("Empieza setup en %lu ms...", millis());

  //---------------------------------------------- WIFI Setup
    Serial.printf("\nConnecting to %s:\n", _wifiSsid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(_wifiSsid, _wifiPassword);
    while (WiFi.status() != WL_CONNECTED) {
      delay(200);
      Serial.print(".");
    }
    Serial.printf("\nWiFi connected, IP address: %s\n", 
      objWifi.localIP().toString().c_str());

  //---------------------------------------------- MQTT Setup
    ptrMqtt->setServer(_mqttServer, 1883);
    ptrMqtt->setBufferSize(512); 
    ptrMqtt->setCallback(mqttCallback);
    MqttConnect();

  //---------------------------------------------- Setup summary
    Serial.printf("Identificador placa: %s\n", espId);
    Serial.printf("Topic publicacion  : %s\n", mqttTopicPub);
    Serial.printf("Topic subscripcion : %s\n", mqttTopicsSub[0]);
    Serial.printf("Termina setup en %lu ms\n\n",millis());

  return 0;
}


int Infra::Loop()
{
  ptrMqtt->loop(); // para que la librería recupere el control
  return 0;
}

Infra::~Infra()
{
  ;
}

void Infra::MqttConnect()
{
  const char* mqtt_user = "infind";
  const char* mqtt_pass = "zancudo";
    
  // Loop until we're reconnected
    while (!ptrMqtt->connected()) {
      Serial.print("Attempting MQTT connection...");
      // Attempt to connect
      if (ptrMqtt->connect(espId, _mqttUser, _mqttPassword)) {
        Serial.printf(" conectado a broker: %s\n", _mqttServer);
        ptrMqtt->subscribe(mqttTopicsSub[0]);
    } else {
      Serial.printf("failed, rc=%d  try again in 5s\n", ptrMqtt->state());
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
    

void Infra::MqttPublish(char *message)
{
  //char Buffer[MQTT_MAX_MESSAGE];

  if (!ptrMqtt->connected())
    MqttConnect();

  if (strlen(message) < MQTT_MAX_MESSAGE)
  { 
    Serial.println(message);
    ptrMqtt->publish(mqttTopicPub, message);
  }
}

#endif
