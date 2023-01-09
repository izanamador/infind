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


//---- Topics de publicación
static char strTopicPubConex_[100];
static char strTopicPubDatos_[100];
static char strTopicPubStLed_[100];
static char strTopicPubStSwi_[100];
 //--- Topics de suscripción individual
static char strTopicSubConfig_[100];
static char strTopicSubCmdLed_[100];
static char strTopicSubCmdSwi_[100];
static char strTopicSubCmdOta_[100];
 //--- Topics de suscripción grupal
static char strTopicAllConfig_[100];
static char strTopicAllCmdLed_[100];
static char strTopicAllCmdSwi_[100];
static char strTopicAllCmdOta_[100];
 //--- Topics de suscripción grupal
static char strTopicPubJuegos_[100];
static char strTopicAllJuegos_[100];

static long stSwitch=HIGH;
//bool bEstandar_; // si la configuración de pines se ajusta a la especificación



void MqttCallback(char* strTopicMsg, byte* payload, unsigned int length)
{
  //----- Prepara la memoria para copiar el mensaje
    char *strMsg = (char *)malloc(length+1);
    strncpy(strMsg, (char*)payload, length);
    strMsg[length]='\0';

  //----- Ver qué llegó
    Serial.printf("Topic:%s, Mensaje=%s\n", strTopicMsg, strMsg);

  //----- Procesamiento del mensaje en función del topic
  if ((strcmp(strTopicMsg, strTopicSubCmdSwi_)==0 ||
       strcmp(strTopicMsg, strTopicAllCmdSwi_)==0) 
     )// && bEstandar_)
  {
    stSwitch = !stSwitch; // statSwi==LOW ? HIGH: LOW
    digitalWrite(PIN_COMUN_SWITCH, stSwitch);  // write to led pin
  }

  //----- Liberación de la memoria reservada
  free(strMsg);
}


EspInfInd::EspInfInd()
{
  // hardcodes relacionados con la placa
    sprintf(espId, "ESP_%d", ESP.getChipId());
//    bEstandar_ = bEstandar;

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



void EspInfInd::setup() 
{
  //---------------------------------------------- ESP Setup
    Serial.begin(ESP_BAUD_RATE);
    Serial.println();
    Serial.printf("Empieza setup en %lu ms...", millis());

  //-------------------------------- Configurar los pines comunes
    //if (bEstandar_) {
      pinMode(PIN_COMUN_LED,        OUTPUT);      
      pinMode(PIN_COMUN_SWITCH,     OUTPUT); 
    //}


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

void EspInfInd::loop()
{
   ptrMqtt->loop(); // para que la librería recupere el control
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