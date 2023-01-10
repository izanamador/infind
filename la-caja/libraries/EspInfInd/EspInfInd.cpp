#include "Arduino.h"
#include "EspInfInd.h"


//--------------------------------------------- MQTT

#define MQTT_SERVER           "iot.ac.uma.es"
#define MQTT_PORT             1883
#define MQTT_BUFFER_SIZE      512
#define MQTT_USER             "II3"
#define MQTT_PASSWORD         "qW30SImD"
#define MQTT_RETRY_DELAY      5000





//----- REQ.MQ1 
#define TOPIC_SUB_CONFIG          "II3/%s/%s/config"
#define TOPIC_SUB_CMDLED          "II3/%s/%s/led/cmd"
#define TOPIC_SUB_CMDSWI          "II3/%s/%s/switch/cmd"
#define TOPIC_SUB_CMDOTA          "II3/%s/%s/FOTA"

//----- REQ.MQ28 entre II3 y ESP se crea un subcampo para comodines en NodeRED
#define TOPIC_PUB_CONEX           "II3/CONEXION/%s/%s/conexion"
#define TOPIC_PUB_DATOS           "II3/DATOS/%s/%s/datos"
#define TOPIC_PUB_STLED           "II3/STLED/%s/%s/led/status"
#define TOPIC_PUB_STSWI           "II3/STSWI/%s/%s/switch/status"

//----- REQ.MQ29 orden grupal a todos los dispositivos
#define TOPIC_ALL_CONFIG          "II3/ALL/config"
#define TOPIC_ALL_CMDLED          "II3/ALL/led/cmd"
#define TOPIC_ALL_CMDSWI          "II3/ALL/switch/cmd"
#define TOPIC_ALL_CMDOTA          "II3/ALL/FOTA"

//----- Topics genéricos para los juegos
#define TOPIC_PUB_JUEGOS          "II3/JUEGO/%s/%s/status"
#define TOPIC_ALL_JUEGOS          "II3/ALL/juegos"







EspInfInd::EspInfInd(const char *strBoardName, bool bStandard) {
  // hardcodes relacionados con la placa
    stSwitch_  = 0;
    strcpy(strBoardName_, strBoardName);
    bStandard_ = bStandard;
    sprintf(espId, "ESP_%d", ESP.getChipId());


  //------ Creación de nombres de topics
    sprintf(strTopicSubConfig_, TOPIC_SUB_CONFIG,strBoardName, espId);
    sprintf(strTopicSubCmdLed_, TOPIC_SUB_CMDLED,strBoardName, espId);
    sprintf(strTopicSubCmdSwi_, TOPIC_SUB_CMDSWI,strBoardName, espId);
    sprintf(strTopicSubCmdOta_, TOPIC_SUB_CMDOTA,strBoardName, espId);
    sprintf(strTopicPubConex_ , TOPIC_PUB_CONEX, strBoardName, espId);
    sprintf(strTopicPubDatos_ , TOPIC_PUB_DATOS, strBoardName, espId);
    sprintf(strTopicPubStLed_ , TOPIC_PUB_STLED, strBoardName, espId);
    sprintf(strTopicPubStSwi_ , TOPIC_PUB_STSWI, strBoardName, espId);
    sprintf(strTopicAllConfig_, TOPIC_ALL_CONFIG,strBoardName, espId);
    sprintf(strTopicAllCmdLed_, TOPIC_ALL_CMDLED,strBoardName, espId);
    sprintf(strTopicAllCmdSwi_, TOPIC_ALL_CMDSWI,strBoardName, espId);
    sprintf(strTopicAllCmdOta_, TOPIC_ALL_CMDOTA,strBoardName, espId);
    sprintf(strTopicPubJuegos_, TOPIC_PUB_JUEGOS,strBoardName, espId);
    sprintf(strTopicAllJuegos_, TOPIC_ALL_JUEGOS,strBoardName, espId);

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

void EspInfInd::MqttSend(char* strTopic, char* strGameStatus, const char *strSrc) {
  static char strSerialized[JSON_MESSAGE_SIZE];

  jsonPub["ChipId"] = espId;
  jsonPub["BoardName"] = strBoardName_;
  jsonPub["Online"] = 1; // true;
  jsonPub["Switch"] = stSwitch_;
  jsonPub["Origin"] = strSrc;
  jsonPub["MqttId"] = "PTE";
  jsonPub["UpTime"] = millis();
  jsonPub["Info"] = strGameStatus;

  serializeJson(jsonPub,strSerialized);
  Serial.printf("Enviando: %s a %s\n---\n%s\n---\n", strGameStatus, strTopic, strSerialized);

  ptrMqtt->publish(strTopic, strSerialized);
}


EspInfInd::~EspInfInd()
{
  ; 
}

//----- REQ.MQ3 LastWill
//#define MQTT_LASTWILL         "{\"online\":false}"
//#define MQTT_LASTWILL         "{\"online\": 0}"

void EspInfInd::MqttConnect()
{

  const char* mqtt_user = "II3";
  const char* mqtt_pass = "qW30SImD";

  
  // Loop until we're reconnected
    while (!ptrMqtt->connected()) 
    {
      Serial.print("Attempting MQTT connection...");

      //---- REQ.MQ3 LastWill
      char strLastWill[100];
      sprintf(strLastWill, "{\"ChipId\": \"%s\", \"BoardName\": \"%s\", \"Online\": 0, \"Source\": \"%s\"}", 
          espId, strBoardName_, STR_ORG_BOARD);
      Serial.printf("LastWill: %s", strLastWill);

      if (ptrMqtt->connect(espId, MQTT_USER, MQTT_PASSWORD, 
          strTopicPubConex_, 1, true, strLastWill)) 
      {
        Serial.printf(" conectado a broker: %s\n", MQTT_SERVER);
        ptrMqtt->subscribe(strTopicSubCmdSwi_);
        ptrMqtt->subscribe(strTopicAllCmdSwi_);
        
        
        //---- REQ.BD4 conexión
          delay(10000); // dar tiempo a que llegue el mensaje de last will
          MqttSend(strTopicPubConex_, (char *)"Connected", STR_ORG_BOARD);
      } 
      else 
      {
        Serial.printf("failed, rc=%d  try again in 5s\n", ptrMqtt->state());
        delay(MQTT_RETRY_DELAY); // Wait 5 seconds before retrying
      }
    } // while
}