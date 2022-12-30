#include "Arduino.h"
#include "infra.h"

Infra::Infra()
{
  // hardcodes relacionados con la placa
    sprintf(espId, "ESP_%d", ESP.getChipId());

  // hardcodes relacionados con el wifi
    // sprintf(_wifiSsid, "infind");
    // sprintf(_wifiPassword, "1518wifi");
    
  // hardcodes relacionados con MQTT  
    // sprintf(_mqttServer, MQTT_SERVER);
    // sprintf(_mqttUser, MQTT_USER);
    // sprintf(_mqttPassword, MQTT_PASSWORD);

    ptrMqtt = new PubSubClient(objWifi);
    for (int i=0; i<TOPIC_NUM_MAX; i++)
    {
      mqttTopicsPub[i] = NULL;
      mqttTopicsSub[i] = NULL;
    } 
}

void OTA_CB_Start(){Serial.println("Nuevo Firmware encontrado. Actualizando...");}
void OTA_CB_Error(int e) {
  char cadena[64];
  snprintf(cadena,64,"ERROR: %d",e);
  Serial.println(cadena); 
}
void OTA_CB_Progress(int x, int todo) {
  char cadena[256];
  int progress=(int)((x*100)/todo);
  if(progress%10==0)
  {
    snprintf(cadena,256,"Progreso: %d%% - %dK de %dK",progress,x/1024,todo/1024);
    Serial.println(cadena);
  }
}
void OTA_CB_End() {Serial.println("Fin OTA. Reiniciando...");}



int Infra::Setup(void (*mqttCallback)(char*, byte*, unsigned int))
{
  //---------------------------------------------- Arduino Pin Mode Setup
  pinMode(LED_BUILTIN, OUTPUT);

  //---------------------------------------------- ESP Setup
  Serial.begin(ESP_BAUD_RATE);
  Serial.println();
  Serial.printf("Empieza setup en %lu ms...", millis());

  //---------------------------------------------- WIFI Setup
    Serial.printf("\nConnecting to %s:\n", WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(WIFI_RETRY_DELAY);
      Serial.print(".");
    }
    Serial.printf("\nWiFi connected, IP address: %s\n", 
      objWifi.localIP().toString().c_str());

  //---------------------------------------------- OTA Setup
    Serial.println( "--------------------------");  
    Serial.println( "Comprobando actualización:");
    Serial.print(OTA_HTTP_ADDRESS);
    Serial.print(":");
    Serial.print(OTA_HTTP_PORT);
    Serial.println(OTA_HTTP_PATH);
    Serial.println("--------------------------");  
    ESPhttpUpdate.setLedPin(ESP_LED_OTA, LOW);
    ESPhttpUpdate.onStart(OTA_CB_Start);
    ESPhttpUpdate.onError(OTA_CB_Error);
    ESPhttpUpdate.onProgress(OTA_CB_Progress);
    ESPhttpUpdate.onEnd(OTA_CB_End);
    switch(ESPhttpUpdate.update(objWifi, OTA_HTTP_ADDRESS, OTA_HTTP_PORT, OTA_HTTP_PATH, OTA_HTTP_VERSION)) {
    case HTTP_UPDATE_FAILED:
      Serial.printf(" HTTP update failed: Error (%d): %s\n", 
          ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println(F(" El dispositivo ya está actualizado"));
      break;
    case HTTP_UPDATE_OK:
      Serial.println(F(" OK"));
      break;
    }

  //---------------------------------------------- MQTT Setup
    ptrMqtt->setServer(MQTT_SERVER, MQTT_PORT);
    ptrMqtt->setBufferSize(MQTT_BUFFER_SIZE); 
    ptrMqtt->setCallback(mqttCallback);
    MqttConnect();
    Serial.printf("\nWiFi connected, IP address: %s\n", 
      objWifi.localIP().toString().c_str());

  //---------------------------------------------- Setup summary
    Serial.printf("Identificador placa: %s\n", espId);
    for (int i=0; i<TOPIC_NUM_MAX; i++) 
    {
      if (mqttTopicsPub[i] != NULL)
        Serial.printf("TopicPub[%d]: %s\n", i, mqttTopicsPub[i]);
      if (mqttTopicsSub[i] != NULL)
        Serial.printf("TopicSub[%d]: %s\n", i, mqttTopicsSub[i]);
    }
    Serial.printf("Termina setup en %lu ms\n\n",millis());
    PrintConfig();

  return 0;
}

void Infra::PrintConfig()
{
  char strAux[JSON_MESSAGE_SIZE];
  serializeJsonPretty(objConfig, strAux);
  Serial.println(strAux);
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
  const char* mqtt_user = "II3";
  const char* mqtt_pass = "qW30SImD";
    
  // Loop until we're reconnected
    while (!ptrMqtt->connected()) 
    {
      Serial.print("Attempting MQTT connection...");
      if (ptrMqtt->connect(espId, MQTT_USER, MQTT_PASSWORD, 
          mqttTopicsPub[TOPIC_MAIN], 1, true, MQTT_LASTWILL)) 
      {
        Serial.printf(" conectado a broker: %s\n", MQTT_SERVER);
        for (int i=0; i<TOPIC_NUM_MAX; i++) 
        {
          if (mqttTopicsSub[i]!=NULL)
            ptrMqtt->subscribe(mqttTopicsSub[i]);
        }
        ptrMqtt->publish(mqttTopicsPub[TOPIC_MAIN], MQTT_CONNECT_MSG, true);
      } 
      else 
      {
        Serial.printf("failed, rc=%d  try again in 5s\n", ptrMqtt->state());
        delay(MQTT_RETRY_DELAY); // Wait 5 seconds before retrying
      }
    } // while
}
    
 

void Infra::MqttPublish(char *message)
{
  //char Buffer[MQTT_MAX_MESSAGE];

  if (!ptrMqtt->connected())
    MqttConnect();

//  if (strlen(message) < MQTT_MAX_MESSAGE)
//  {
  Serial.print("MqttPublish: ");
  Serial.print(message);
  Serial.print("\n");    
  ptrMqtt->publish(mqttTopicsPub[TOPIC_MAIN], message);
//  }
}
