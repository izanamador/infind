-------------- II3/CONEXION/#    esp001/ESP_14287505/conexion
--------------------- II3/GAME/STATUS/#   esp001/ESP_14287505
Nodered
- timestamp   :
- msgtype     : connection
--------------------------- 
Arduino
- boardname   : esp001
- online      : 0 o 1
- activeface  : 0 a 6
- userinfo    : 
- espid       : ESP_14287505
- source      : STR_ORG_BOARD o STR_ORG_MQTT
- uptime


0 #fbf9f9 blanco
1 #3206e5 azul
2 green   verde
3 #d40808 rojo
4 #121212 negro
5 #840cc6 violeta


0
1
4
5
6
8


["cfPerStat"] = cfPerStat_;
["cfPerFota"] = cfPerFota_;

["cfSwLight"] = cfSwLight_;
["stSwLevel"] = stSwLevel_;
["uiSwLevel"] = (stSwLevel_== cfSwLight_) ? 1 : 0;
["cfLdLight"] = cfLdLight_;
["stLdLevel"] = stLdLevel_;
["uiLdLevel"] = (stLdLevel_== cfLdLight_) ? 1 : 0;
["cfLedBrig"] = cfLedBrig_;
["cfLedVelo"] = cfLedVelo_;

["Origin"] = strSrc;
["MqttId"] = "PTE";
["UpTime"] = millis();
["Info"] = strGameStatus;

--------------------- II3/DATOS/esp001/ESP_14287505/datos

gamestatus --------------------- II3/GAME/STATUS/#   esp001/ESP_14287505

Arduino

  - gamestate: 
            STAT_NEWMATCH STAT_WAITSTART STAT_NEWGAME STAT_NEWSET 
            STAT_PLAYING  STAT_ONHOLD    STAT_LOST    STAT_WON




  jsonPub["cfPerStat"] = cfPerStat_;
  jsonPub["cfPerFota"] = cfPerFota_;

  jsonPub["cfSwLight"] = cfSwLight_;
  jsonPub["stSwLevel"] = stSwLevel_;
  jsonPub["uiSwLevel"] = (stSwLevel_== cfSwLight_) ? 1 : 0;
  
  jsonPub["cfLdLight"] = cfLdLight_;
  jsonPub["stLdLevel"] = stLdLevel_;
  jsonPub["uiLdLevel"] = (stLdLevel_== cfLdLight_) ? 1 : 0;
  jsonPub["cfLedBrig"] = cfLedBrig_;
  jsonPub["cfLedVelo"] = cfLedVelo_;

  jsonPub["MqttId"] = "PTE";
  jsonPub["UpTime"] = millis();
  jsonPub["Info"] = strGameStatus;

//------------------------- 
command
    maxtimegame
    maxtimeset
    maxnumsets
    
    maxtmset

    gametime

---------------------------
void EspInfInd::MqttSend(char* strTopic, char* strGameStatus, const char *strSrc) {
  static char strSerialized[JSON_MESSAGE_SIZE];

  jsonPub["boardname"] = strBoardName_;
  jsonPub["online"] = 1; // true;
  jsonPub["espid"] = espId;
  jsonPub["source"] = strSrc;


  jsonPub["cfPerStat"] = cfPerStat_;
  jsonPub["cfPerFota"] = cfPerFota_;

  jsonPub["cfSwLight"] = cfSwLight_;
  jsonPub["stSwLevel"] = stSwLevel_;
  jsonPub["uiSwLevel"] = (stSwLevel_== cfSwLight_) ? 1 : 0;
  
  jsonPub["cfLdLight"] = cfLdLight_;
  jsonPub["stLdLevel"] = stLdLevel_;
  jsonPub["uiLdLevel"] = (stLdLevel_== cfLdLight_) ? 1 : 0;
  jsonPub["cfLedBrig"] = cfLedBrig_;
  jsonPub["cfLedVelo"] = cfLedVelo_;

  jsonPub["MqttId"] = "PTE";
  jsonPub["UpTime"] = millis();
  jsonPub["Info"] = strGameStatus;

  --------------------------------------------------------------
  //------------ Parámetros de salida
  pEsp_->jsonPub["GameStat"] = GameStat;
  pEsp_->jsonPub["NumFails"] = NumFails;
  pEsp_->jsonPub["TmTotPly"] = TmTotPly;
  pEsp_->jsonPub["TmActive"] = TmActive;
  pEsp_->jsonPub["TmOnHold"] = TmOnHold;
  pEsp_->jsonPub["TmLstTry"] = TmLstTry;
  pEsp_->jsonPub["GameInfo"] = strGameInfoOut;
  pEsp_->jsonPub["Time2End"] = (String)Time2End;

  pEsp_->jsonPub["Origin"] = STR_ORG_BOARD;
  pEsp_->jsonPub["MqttId"] = "";
  pEsp_->jsonPub["UpTime"] = millis();

  //------------ Datos specíficos del estado del juego
  pEsp_->jsonPub["FaceNumb"] = FaceNumb;
  pEsp_->jsonPub["GameName"] = GameName;

  //------------ Parámetros de entrada
  pEsp_->jsonPub["GameTime"] = GameTime;
  pEsp_->jsonPub["FailTime"] = FailTime;
  pEsp_->jsonPub["NumTries"] = NumTries;
  pEsp_->jsonPub["GameParm"] = GameParm;

