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
}

int Setup(char *topicPub, char **topicsSub, void (*callback)(char*, byte*, unsigend int));


int Infra::Setup(char *topicPub, char **topicsSub, void (*callback)(char*, byte*, unsigend int))
{
  //---------------------------------------------- ESP Setup
  Serial.begin(115200);
  Serial.println();
  Serial.println("Empieza setup en %lu ms...", millis());
  pinMode(LED1, OUTPUT);    // inicializa GPIO como salida
  pinMode(LED2, OUTPUT);    

  //---------------------------------------------- WIFI Setup
    Serial.printf("\nConnecting to %s:\n", _wifiSsid);
    objWifi.mode(WIFI_STA);
    objWifi.begin(_wifiSsid, _wifiPassword);
    while (objWifi.status() != WL_CONNECTED) {
      delay(200);
      Serial.print(".");
    }
    Serial.printf("\nWiFi connected, IP address: %s\n", 
      objWifi.localIP().toString().c_str());

  //---------------------------------------------- MQTT Setup
    objMqtt.setServer(_mqttServer, 1883);
    objMqtt.setBufferSize(512); 
    mqttTopicPub = topicPub;
    mqttTopicsSub = topicsSub;
    objMqtt.setCallback(mqttCallback);
    MqttConnect();

  //---------------------------------------------- Setup summary
    Serial.printf("Identificador placa: %s\n", _boardId );
    Serial.printf("Topic publicacion  : %s\n", _mqttTopicPub);
    Serial.printf("Topic subscripcion : %s\n", _mqttTopicSub);
    Serial.printf("Termina setup en %lu ms\n\n",millis());



  return 0;
}


int Infra::Loop()
{
  objMqtt.loop(); // para que la librería recupere el control
  return 0;
}

void Infra::~Infra()
{
  ;
}

void Infra::MqttConnect()
{
  const char* mqtt_user = "infind";
  const char* mqtt_pass = "zancudo";
    
  // Loop until we're reconnected
    while (!objMqtt.connected()) {
      Serial.print("Attempting MQTT connection...");
      // Attempt to connect
      if (objMqtt.connect(_boardId, _mqttUser, _mqttPassword)) {
        Serial.printf(" conectado a broker: %s\n", _mqttServer);
        objMqtt.subscribe(mqttTopicsSub[0]);
    } else {
      Serial.printf("failed, rc=%d  try again in 5s\n", objMqtt.state());
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
    

void Infra::MqttPublish(char *message)
{
  char Buffer[MQTT_MAX_MESSAGE];

  if (!objMqtt.connected)
    MqttConnect();

  if (len(message) < MQTT_MAX_MESSAGE)
  { 
    Serial.println(message);
    objMqtt.publish(mqttTopicPub, Buffer);
  }
}