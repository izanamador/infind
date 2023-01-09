#include "Arduino.h"
#include "EspInfInd.h"


//--------------------------------------------- MQTT

#define MQTT_SERVER           "iot.ac.uma.es"
#define MQTT_PORT             1883
#define MQTT_BUFFER_SIZE      512
#define MQTT_USER             "II3"
#define MQTT_PASSWORD         "qW30SImD"
#define MQTT_RETRY_DELAY      5000


//----- REQ.MQ3 LastWill
#define MQTT_LASTWILL         "{\"online\":false}"
#define MQTT_CONNECT_MSG      "{\"online\":true}"




//----- REQ.MQ1 
#define TOPIC_SUB_CONFIG          "II3/%s/config"
#define TOPIC_SUB_CMDLED          "II3/%s/led/cmd"
#define TOPIC_SUB_CMDSWI          "II3/%s/switch/cmd"
#define TOPIC_SUB_CMDOTA          "II3/%s/FOTA"

//----- REQ.MQ28 entre II3 y ESP se crea un subcampo para comodines en NodeRED
#define TOPIC_PUB_CONEX           "II3/CONEX/%s/conexion"
#define TOPIC_PUB_DATOS           "II3/DATOS/%s/datos"
#define TOPIC_PUB_STLED           "II3/STLED/%s/led/status"
#define TOPIC_PUB_STSWI           "II3/STSWI/%s/switch/status"

//----- REQ.MQ29 orden grupal a todos los dispositivos
#define TOPIC_ALL_CONFIG          "II3/ALL/config"
#define TOPIC_ALL_CMDLED          "II3/ALL/led/cmd"
#define TOPIC_ALL_CMDSWI          "II3/ALL/switch/cmd"
#define TOPIC_ALL_CMDOTA          "II3/ALL/FOTA"

//----- Topics genéricos para los juegos
#define TOPIC_PUB_JUEGOS          "II3/JUEGO/%s/status"
#define TOPIC_ALL_JUEGOS          "II3/ALL/juegos"







EspInfInd::EspInfInd(bool bStandard) {
  // hardcodes relacionados con la placa
    stSwitch_  = 0;
    bStandard_ = bStandard;
    sprintf(espId, "ESP_%d", ESP.getChipId());


  //------ Creación de nombres de topics
    sprintf(strTopicSubConfig_, TOPIC_SUB_CONFIG,espId);
    sprintf(strTopicSubCmdLed_, TOPIC_SUB_CMDLED,espId);
    sprintf(strTopicSubCmdSwi_, TOPIC_SUB_CMDSWI,espId);
    sprintf(strTopicSubCmdOta_, TOPIC_SUB_CMDOTA,espId);
    sprintf(strTopicPubConex_ , TOPIC_PUB_CONEX, espId);
    sprintf(strTopicPubDatos_ , TOPIC_PUB_DATOS, espId);
    sprintf(strTopicPubStLed_ , TOPIC_PUB_STLED, espId);
    sprintf(strTopicPubStSwi_ , TOPIC_PUB_STSWI, espId);
    sprintf(strTopicAllConfig_, TOPIC_ALL_CONFIG,espId);
    sprintf(strTopicAllCmdLed_, TOPIC_ALL_CMDLED,espId);
    sprintf(strTopicAllCmdSwi_, TOPIC_ALL_CMDSWI,espId);
    sprintf(strTopicAllCmdOta_, TOPIC_ALL_CMDOTA,espId);
    sprintf(strTopicPubJuegos_, TOPIC_PUB_JUEGOS,espId);
    sprintf(strTopicAllJuegos_, TOPIC_ALL_JUEGOS,espId);

  //------- Inicialización de objetos
    ptrMqtt = new PubSubClient(objWifi); 
}


void EspInfInd::Setup(void (*MqttCallback)(char*, byte*, unsigned int)) {
  
  //---------------------------------------------- ESP Setup
    Serial.begin(ESP_BAUD_RATE);
    Serial.println();
    Serial.printf("Empieza setup en %lu ms...", millis());

  //-------------------------------- Configurar los pines comunes
    if (bStandard_) {
      pinMode(PIN_COMUN_LED,        OUTPUT);      
      pinMode(PIN_COMUN_SWITCH,     OUTPUT); 
    }


  //---------------------------------------------- WIFI Setup
    Serial.printf("\nConnecting to %s:\n", WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(WIFI_RETRY_DELAY);
      Serial.print(".");
    }
    Serial.printf("\nWiFi connected, IP address: %s\n", 
      objWifi.localIP().toString().c_str());

  // delay(1000);
    
  //---------------------------------------------- MQTT Setup
    ptrMqtt->setServer(MQTT_SERVER, MQTT_PORT);
    ptrMqtt->setBufferSize(MQTT_BUFFER_SIZE); 
    ptrMqtt->setCallback(MqttCallback);
    MqttConnect();
}

void EspInfInd::Loop()
{
   ptrMqtt->loop(); // para que la librería recupere el control

} 


//void EspInfInd::callback(char* topic, byte* payload, unsigned int length)
//{
//  /* Procesa los mensajes enviados por Node-red */
//  char *mensaje = (char *)malloc(length+1);
//  strncpy(mensaje, (char*)payload, length);
//  mensaje[length]='\0';
//  //if (strcmp(topic, strTopicCmd)==0){
//  //  ans = atoi(mensaje);
//  //  objInfra.ReportStart(NULL);}
//  free(mensaje);  
//}

void EspInfInd::MqttReceived(char* strTopic, byte* payload, unsigned int length)
{

  //----- Prepara la memoria para copiar el mensaje
    char *strMsg = (char *)malloc(length+1);
    strncpy(strMsg, (char*)payload, length);
    strMsg[length]='\0';
    deserializeJson(jsonSub,strMsg);
    Serial.printf("Test1: %s", (const char *)jsonSub["Test1"]);
    //Serial.print(jsonSub["Test1"].c_str());

  //----- Ver qué llegó
    Serial.printf("Topic:%s, Mensaje=%s\n", strTopic, strMsg);

  //----- Procesamiento del mensaje en función del topic
  if ((strcmp(strTopic, strTopicSubCmdSwi_)==0 ||
       strcmp(strTopic, strTopicAllCmdSwi_)==0) 
      && bStandard_)
  {
//    char strOut[100];
    stSwitch_ =  stSwitch_==LOW ? HIGH: LOW;
    digitalWrite(PIN_COMUN_SWITCH, stSwitch_);  // write to led pin
//    sprintf(strOut,"{\"CHIPID\":\"%s\",\"SWITCH\":%d,\"origen\":\"mqtt\",\"id\":\"%s\"}",
//      espId, stSwitch_, "pte");
//    ptrMqtt->publish(strTopicPubStSwi_, strOut);
    MqttSend(strTopicPubStSwi_, NULL);
  }

  //----- Liberación de la memoria reservada
  free(strMsg);
}

void EspInfInd::MqttSend(char* strTopic, char* strGameStatus) {
  static char strSerialized[JSON_MESSAGE_SIZE];

  jsonPub["ChipId"] = espId;
  jsonPub["Online"] = 1; // true;
  jsonPub["Switch"] = stSwitch_;
  jsonPub["Origin"] = "mqtt";
  jsonPub["MqttId"] = "PTE";
  jsonPub["UpTime"] = millis();

  serializeJson(jsonPub,strSerialized);
  ptrMqtt->publish(strTopic, strSerialized);
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
        ptrMqtt->subscribe(strTopicSubCmdSwi_);
        ptrMqtt->subscribe(strTopicAllCmdSwi_);
        
        
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