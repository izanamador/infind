#include "Arduino.h"
#include "EspInfInd.hpp"


//--------------------------------------------- MQTT

#define MQTT_SERVER           "iot.ac.uma.es"
#define MQTT_PORT             1883
#define MQTT_BUFFER_SIZE      512
#define MQTT_USER             "II3"
#define MQTT_PASSWORD         "qW30SImD"
#define MQTT_RETRY_DELAY      5000

//----- Topics específicos de juegos
#define TOPIC_SUBGAME_CMD        "II3/CMD/%s/ESP%s"
#define TOPIC_SUB_GAME_EST        "II3/EST/%s/ESP%s"

//----- REQ.MQ3 LastWill
#define MQTT_LASTWILL         "{\"online\":false}"
#define MQTT_CONNECT_MSG      "{\"online\":true}"

//----- REQ.MQ1 
#define TOPIC_SUB_FMT_CONFIG      "II3/%s/config"
#define TOPIC_SUB_LED_CMD         "II3/%s/led/cmd"
#define TOPIC_SUB_SWITCH_CMD      "II3/%s/switch/cmd"
#define TOPIC_SUB_FOTA            "II3/%s/FOTA"

//----- REQ.MQ28 entre II3 y ESP se crea un subcampo para comodines en NodeRED
//#define TOPIC_PUB_FMT_CONEXION    "II3/CON/%s/conexion"
#define TOPIC_PUB_FMT_CONEXION    "II3/%s/conexion"
#define TOPIC_PUB_FMT_DATOS       "II3/DAT/%s/datos"
#define TOPIC_PUB_LED_STATUS      "II3/LED/%s/led/status"
#define TOPIC_PUB_SWITCH_STATUS   "II3/SWI/%s/switch/status"

//----- REQ.MQ29 orden grupal a todos los dispositivos
#define TOPIC_SUB_ALL_FMT_CONFIG  "II3/all/config"
#define TOPIC_SUB_ALL_LED_CMD     "II3/all/led/cmd"
#define TOPIC_SUB_ALL_SWITCH_CMD  "II3/all/switch/cmd"
#define TOPIC_SUB_ALL_FOTA        "II3/all/FOTA"



EspInfInd::EspInfInd()
{
  // hardcodes relacionados con la placa
    sprintf(espId, "ESP_%d", ESP.getChipId());
    sprintf(strTopicPubConex_,TOPIC_PUB_FMT_CONEXION,espId);
    sprintf(strTopicSubConf_ ,TOPIC_SUB_FMT_CONFIG, espId);


    ptrMqtt = new PubSubClient(objWifi);
  
}



void MqttCallback(char* topic, byte* payload, unsigned int length)
{
  /* Procesa los mensajes enviados por Node-red */
  char *mensaje = (char *)malloc(length+1);
  strncpy(mensaje, (char*)payload, length);
  mensaje[length]='\0';
  if (strcmp(topic, strTopicCmd)==0){
    ans = atoi(mensaje);
    objInfra.ReportStart(NULL);}
  free(mensaje);
}



int EspInfInd::Setup() 
{
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

delay(1000);
/*
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
*/
    
  //---------------------------------------------- MQTT Setup
    ptrMqtt->setServer(MQTT_SERVER, MQTT_PORT);
    ptrMqtt->setBufferSize(MQTT_BUFFER_SIZE); 
    ptrMqtt->setCallback(MqttCallback);
    MqttConnect();

    /*
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
*/

  //---------------------------------------------- Status of the game
    /*
    numTries_ = 0;
    milLsRep_ = 0;
    milStart_ = 0;
    milTries_ = 0;
    currStat_ = STAT_SETUP;
    ReportStatus(NULL);  
    currStat_ = STAT_WAIT;
*/
  return 0;
}

int EspInfInd::Loop()
{
   ptrMqtt->loop(); // para que la librería recupere el control
   return 0;
} 

EspInfInd::~EspInfInd()
{
  ;
}




void EspInfInd::MqttConnect()
{

  const char* mqtt_user = "II3";
  const char* mqtt_pass = "qW30SImD";

  
  // Loop until we're reconnected
    while (!ptrMqtt->connected()) 
    {
      Serial.print("Attempting MQTT connection...");

      //---- REQ.MQ3 LastWill
      if (ptrMqtt->connect(espId, MQTT_USER, MQTT_PASSWORD, 
          strTopicPubConex_, 1, true, MQTT_LASTWILL)) 
      {
        Serial.printf(" conectado a broker: %s\n", MQTT_SERVER);
        ptrMqtt->subscribe(strTopicSubConf_);
        //for (int i=0; i<TOPIC_NUM_MAX; i++) 
        //{
        //  if (mqttTopicsSub[i]!=NULL)
        //    ptrMqtt->subscribe(mqttTopicsSub[i]);
        //}

    		//---- REQ.BD4 conexión 
    			ptrMqtt->publish(strTopicPubConex_, MQTT_CONNECT_MSG, true);
      } 
      else 
      {
        Serial.printf("failed, rc=%d  try again in 5s\n", ptrMqtt->state());
        delay(MQTT_RETRY_DELAY); // Wait 5 seconds before retrying
      }
    } // while
}
    

/*
void Esp8266::MqttPublish(char* topic, char *message)
{
  if (!ptrMqtt->connected())
    MqttConnect();

  if (strlen(message) < MQTT_MAX_MESSAGE)
  {
	  Serial.print("MqttPublish: ");
  	Serial.print(message);
  	Serial.print("\n");    
  	ptrMqtt->publish(strTopic, strMessage);
  }
}
*/