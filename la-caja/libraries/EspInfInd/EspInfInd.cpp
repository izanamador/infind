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
#define TOPIC_SUB_CMDSWI          "II3/%s/SETSWITCH/%s/switch/cmd"
#define TOPIC_SUB_CMDOTA          "II3/%s/%s/FOTA"

//----- REQ.MQ28 entre II3 y ESP se crea un subcampo para comodines en NodeRED
#define TOPIC_PUB_CONEX           "II3/CONEXION/%s/%s/conexion"
#define TOPIC_PUB_DATOS           "II3/DATOS/%s/%s/datos"
#define TOPIC_PUB_STLED           "II3/STLED/%s/%s/led/status"
#define TOPIC_PUB_STSWI           "II3/STASWITCH/%s/%s/switch/status"

//----- REQ.MQ29 orden grupal a todos los dispositivos
#define TOPIC_ALL_CONFIG          "II3/ALL/config"
#define TOPIC_ALL_CMDLED          "II3/ALL/led/cmd"
#define TOPIC_ALL_CMDSWI          "II3/ALL/SETSWITCH/switch/cmd"
#define TOPIC_ALL_CMDOTA          "II3/ALL/FOTA"

//----- Topics genéricos para los juegos
#define TOPIC_PUB_JUEGOS          "II3/JUEGO/%s/%s/status"
#define TOPIC_ALL_JUEGOS          "II3/ALL/juegos"







EspInfInd::EspInfInd(const char *strBoardName, bool bStandard) {
  // hardcodes relacionados con la placa
    cfSwitchOn_  = 0;  // configuración por defecto del switch: 0=encendido
    stSwitchLev_ = 0;  // valor inicial del switch (apagado)
    strcpy(strBoardName_, strBoardName);
    bStandard_ = bStandard;
    sprintf(espId, "ESP_%d", ESP.getChipId());


  //------ Creación de nombres de topics
    sprintf(strTopicSubCmdSwi_, TOPIC_SUB_CMDSWI,strBoardName, espId);
    sprintf(strTopicPubStSwi_ , TOPIC_PUB_STSWI, strBoardName, espId);

    sprintf(strTopicSubConfig_, TOPIC_SUB_CONFIG,strBoardName, espId);
    sprintf(strTopicAllConfig_, TOPIC_ALL_CONFIG,strBoardName, espId);

    sprintf(strTopicSubCmdLed_, TOPIC_SUB_CMDLED,strBoardName, espId);
    sprintf(strTopicSubCmdOta_, TOPIC_SUB_CMDOTA,strBoardName, espId);
    sprintf(strTopicPubConex_ , TOPIC_PUB_CONEX, strBoardName, espId);
    sprintf(strTopicPubDatos_ , TOPIC_PUB_DATOS, strBoardName, espId);
    sprintf(strTopicPubStLed_ , TOPIC_PUB_STLED, strBoardName, espId);
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

#define SWITCH_CMD_MSG 1
#define SWITCH_CFG_MSG 2
#define SWITCH_BOTON   3

void EspInfInd::UpdateSwitch(int iUpdateType, long newLevel, long newConfig) {
  char strStatus[100];
  long highLow;


  // Actualización de switch por comando MQTT
    if (iUpdateType == SWITCH_CMD_MSG) { 
      if (newLevel != 0 && newLevel != 1){
        // TODO tratamiento errores
        Serial.printf("Error1 iUpdate=%d, newLevel=%d, newConfig=%d\n", iUpdateType, newLevel, newConfig);

      } else if (newLevel == stSwitchLev_) {
        Serial.printf("Switch permanece en %d\n", stSwitchLev_);

      } else {
        sprintf(strStatus, "Cambiando valor de switch de %d a %d\n", stSwitchLev_, newLevel);
        stSwitchLev_ = newLevel;
        highLow = (stSwitchLev_==0 && cfSwitchOn_==0)||(stSwitchLev_==1 && cfSwitchOn_==1)?LOW:HIGH;
        digitalWrite(PIN_COMUN_SWITCH, highLow);  // write to led pin
        MqttSend(strTopicPubStSwi_, strStatus, STR_ORG_MQTT);
      }  // comando MQTT
  }

  // Actualización de configuración de switch
    else if (iUpdateType == SWITCH_CFG_MSG) {
      if (newConfig == cfSwitchOn_) {
        Serial.printf("New switch config LIGHT=%d remains LIGHT=%d\n", newConfig, cfSwitchOn_);

      } if (newConfig != 0 && newConfig != 1) {
        Serial.printf("Error1 iUpdate=%d, newLevel=%d, newConfig=%d\n", iUpdateType, newLevel, newConfig);
        ; // TODO tratamiento errores
      } else {
        sprintf(strStatus, "Config switch updated to LIGHT=%d from LIGHT=%d\n", newConfig, cfSwitchOn_);
        cfSwitchOn_ = newConfig;
        highLow = (stSwitchLev_==0 && cfSwitchOn_==0)||(stSwitchLev_==1 && cfSwitchOn_==1)?LOW:HIGH;
        digitalWrite(PIN_COMUN_SWITCH, highLow);  // write to led pin
        MqttSend(strTopicPubStSwi_, strStatus, STR_ORG_MQTT);
      }
  } // configuración MQTT

}


void EspInfInd::MqttReceived(char* strTopic, byte* payload, unsigned int length)
{

  //----- Mensaje recibido
    char *strMsg = (char *)malloc(length+1);
    strncpy(strMsg, (char*)payload, length);
    strMsg[length]='\0';
    Serial.printf("\n\n\nRecibido por Topic:%s\n%s\n----------------\n", strTopic, strMsg);
    deserializeJson(jsonSub,strMsg);
    free(strMsg);

  //----- Procesamiento de mensajes hacia el Switch
    char strStatus[100];

    if ((strcmp(strTopic, strTopicSubCmdSwi_)==0 ||
         strcmp(strTopic, strTopicAllCmdSwi_)==0) 
        && bStandard_)
    {
      long lev =jsonSub["level"];
      UpdateSwitch(SWITCH_CMD_MSG, lev, -1);
    }

  //----- Procesamiento de mensajes de configuración
    else if ((strcmp(strTopic, strTopicSubConfig_)==0 ||
              strcmp(strTopic, strTopicAllConfig_)==0)  )
    {
      if (jsonSub["SWITCH"]=="ON=0") {
        UpdateSwitch(SWITCH_CFG_MSG, -1, 0);
      }
      else if (jsonSub["SWITCH"]=="ON=1"){
        UpdateSwitch(SWITCH_CFG_MSG, -1, 1);
      }
      else if (jsonSub["SWITCH"]!="null")
        UpdateSwitch(SWITCH_CFG_MSG, -1, 2);
      else {
        Serial.printf("Configuration remains LIGHT=%d\n", cfSwitchOn_);
      }
    }

  //----- Mensaje no esperado
    else {
      // TODO tratamiento de error
      Serial.printf("Topic no reconocido\n");
    }
}

void EspInfInd::MqttSend(char* strTopic, char* strGameStatus, const char *strSrc) {
  static char strSerialized[JSON_MESSAGE_SIZE];

  jsonPub["ChipId"] = espId;
  jsonPub["BoardName"] = strBoardName_;
  jsonPub["Online"] = 1; // true;
  jsonPub["SwitchLevel"] = stSwitchLev_;
  jsonPub["SwitchCfgOn"] = cfSwitchOn_;
  jsonPub["SwitchLight"] = (stSwitchLev_== cfSwitchOn_)?1:0;

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
        ptrMqtt->subscribe(strTopicSubConfig_);
        //Serial.printf("Suscribiendo a %s\n", strTopicAllConfig_);
        ptrMqtt->subscribe(strTopicAllConfig_);
        
         
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