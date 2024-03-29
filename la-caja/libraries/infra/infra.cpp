#include "Arduino.h"
#include "infra.h"

#define STAT_SETUP    0
#define STAT_WAIT     1
#define STAT_RESET    2
#define STAT_PLAY     3
#define STAT_LOST     4
#define STAT_WON      5
#define STAT_TIMEOUT  6
#define STAT_END      7


Infra::Infra()
{

    sprintf(espId, "ESP_%d", ESP.getChipId());
    ptrMqtt = new PubSubClient(objWifi);
    for (int i=0; i<TOPIC_NUM_MAX; i++)
    {
      mqttTopicsPub[i] = NULL;
      mqttTopicsSub[i] = NULL;
    } 
  
}

int Infra::GameRunning()
{
  return (currStat_== STAT_PLAY);
}



void Infra::ReportStatus(char* GameInfo)
{
  global_game_info = GameInfo;
  bool bReport = false;
  static char message[JSON_MESSAGE_SIZE];
  StaticJsonDocument<JSON_MESSAGE_SIZE> json;

  // never report after completion of the game
  if (currStat_ == STAT_END)
    return;


  // report once if the game is setting up
  if (currStat_ == STAT_SETUP)
    json["state"] = "Setting Up";

  // report periodically if the game is waiting to start
  else if (currStat_ == STAT_WAIT)
  {
    /* bReport = true; */
    json["state"] = "Waiting to start game";
  }

  // report once if the game is about to start
  else if (currStat_ == STAT_RESET)
    json["state"] = "Starting";
  
  // report periodically or forced if info about the game
  else if (currStat_ == STAT_PLAY)
  {
    /* bReport = (GameInfo!=NULL); */
    json["state"] = "Playing";
  }
  // report lost and retry
  else if (currStat_ == STAT_LOST){
    bReport = true;
    json["state"] = "You lose";

    // win
  }else if (currStat_ == STAT_WON){
    bReport = true;
    json["state"] = "You won";
  
  }else if (currStat_ == STAT_TIMEOUT){
    bReport = true;
    json["state"] = "Timeout! You lost";
  }
  
  if (bReport || millis() > 5000+milLsRep_)
  {
    milLsRep_ = millis(); // refresh last report timestamp
    json["gametime"] = millis()-milStart_;
    json["trytime"]  = millis()-milTries_;
    json["numtries"] = numTries_;
    json["gameinfo"] = global_game_info;   
    serializeJson(json,message);
    MqttPublish(message);
    bReport = false;
  }
}

void Infra::ReportStatus2(char* GameInfo){
  global_game_info = GameInfo;
  static char message[JSON_MESSAGE_SIZE];
  StaticJsonDocument<JSON_MESSAGE_SIZE> json;
  milLsRep_ = millis(); // refresh last report timestamp
  json["gametime"] = millis()-milStart_;
  json["trytime"]  = millis()-milTries_;
  json["numtries"] = numTries_;
  json["gameinfo"] = global_game_info;
  serializeJson(json,message);
  MqttPublish(message);
}

void Infra::ReportStart(char* GameInfo)
{
  if (currStat_ == STAT_WAIT)
  {    
    currStat_ = STAT_RESET;
    milStart_ = millis();
    milTries_ = milStart_;
    numTries_ = 1;
    ReportStatus(GameInfo);    
    currStat_ = STAT_PLAY;
  }
}

void Infra::ReportFailure(char* GameInfo)
{
  if (currStat_ == STAT_PLAY)
  {    
    currStat_ = STAT_LOST;
    numTries_++;
    ReportStatus(GameInfo);
    milTries_ = millis();    
    currStat_ = STAT_PLAY;
  }

}

void Infra::ReportSuccess(char* GameInfo)
{
  if (currStat_ == STAT_PLAY)
  {    
    currStat_ = STAT_WON;
    numTries_++;
    ReportStatus(GameInfo);
    currStat_ = STAT_END;
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
    Serial.print(OTA_HTTP_ADDRESS2);
    Serial.print(":");
    Serial.print(OTA_HTTP_PORT);
    Serial.println(OTA_HTTP_PATH);
    Serial.println("--------------------------");  
    ESPhttpUpdate.setLedPin(ESP_LED_OTA, LOW);
    ESPhttpUpdate.onStart(OTA_CB_Start);
    ESPhttpUpdate.onError(OTA_CB_Error);
    ESPhttpUpdate.onProgress(OTA_CB_Progress);
    ESPhttpUpdate.onEnd(OTA_CB_End);
    switch(ESPhttpUpdate.update(objWifi, OTA_HTTP_ADDRESS2, OTA_HTTP_PORT, OTA_HTTP_PATH, OTA_HTTP_VERSION)) {
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

  //---------------------------------------------- Status of the game
    numTries_ = 0;
    milLsRep_ = 0;
    milStart_ = 0;
    milTries_ = 0;
    currStat_ = STAT_SETUP;
    ReportStatus(NULL);  
    currStat_ = STAT_WAIT;

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

  if ((currStat_== STAT_PLAY) && 
      ((millis()-milTries_) > 60000*15) || 
      ((millis()-milStart_) > 60000*60) )
  {
    currStat_ = STAT_TIMEOUT;
    ReportStatus(global_game_info); // reporte periódico
    currStat_ = STAT_END;
  }
  else
  {
    ptrMqtt->loop(); // para que la librería recupere el control
    ReportStatus(global_game_info); // reporte periódico
  }

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

  if (!ptrMqtt->connected())
    MqttConnect();

  Serial.print("MqttPublish: ");
  Serial.print(message);
  Serial.print("\n");    
  ptrMqtt->publish(mqttTopicsPub[TOPIC_MAIN], message);

}

void Infra::RestartBoard(){
  WiFi.disconnect();                                    // Drop current connection
  delay(1000);
  ESP.restart();
}

void Infra::setOTAAddress(const char* Address){
  OTA_HTTP_ADDRESS2 = const_cast<char*>(Address);
}
                          
