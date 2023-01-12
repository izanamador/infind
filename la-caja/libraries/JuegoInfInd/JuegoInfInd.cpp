#include "Arduino.h"
#include "JuegoInfInd.h"


#define STAT_NOMATCH  		0 // BLANCO
#define STAT_WAITSTART     	1 // AZUL
#define STAT_PLAYING     	2 // VERDE
#define STAT_ONHOLD     	3 // ROJO
#define STAT_LOST     		4 // NEGRO
#define STAT_WON      		5 // VIOLETA
char strGameInfoOut[100];
//#define STAT_TIMEOUT  6
//#define STAT_END      7


JuegoInfInd::JuegoInfInd(const char *gameName, int faceNumb, EspInfInd *pEspInfInd) {
    //---  parametros del juego
	strcpy(GameName, gameName);
	sprintf(strGameInfoOut, "Game name: %s", GameName);
	FaceNumb = faceNumb;
	pEsp_ = pEspInfInd;

	//-- inicializaciones auxiliares
    GameStat = STAT_NOMATCH;
	strcpy(GameParm, "");
	strcpy(TopicPub, pEsp_->strTopicGameStatus);
}



JuegoInfInd::~JuegoInfInd() {
  ;
}



void JuegoInfInd::Setup() {
}

void JuegoInfInd::ReportFail(char* strGameInfo){
    NumFails++;
    strcpy(strGameInfoOut, strGameInfo);
	TmLstTry = millis()-msTries_-msTryHolded_;
	ReportStatus(strGameInfoOut);

    msTries_ = millis();
    msTryHolded_ = 0;
    GameStat = STAT_PLAYING;
}

void JuegoInfInd::ReportSuccess(char* strGameInfo){
	sprintf(strGameInfoOut, "Has gandado %s.\n%s", GameName,strGameInfo);
	GameStat = STAT_WON;
	ReportStatus(strGameInfoOut);
}


void JuegoInfInd::ReportStatus (char *strGameInfo) {
  strcpy(strGameInfoOut, strGameInfo);
  char strSerialized[JSON_MESSAGE_SIZE];


  int secs = GameTime - TmActive/1000; // segundos restantes
  sprintf(Time2End, "--%02d:%02d--", secs/60, secs%60);
  
  Serial.printf("\nTime2End %s\n", Time2End);

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

  //------------ Parámetros de salida
  pEsp_->jsonPub["GameStat"] = GameStat;
  pEsp_->jsonPub["NumFails"] = NumFails;
  pEsp_->jsonPub["TmTotPly"] = TmTotPly;
  pEsp_->jsonPub["TmActive"] = TmActive;
  pEsp_->jsonPub["TmOnHold"] = TmOnHold;
  pEsp_->jsonPub["TmLstTry"] = TmLstTry;
  pEsp_->jsonPub["========"] = "========---";
  pEsp_->jsonPub["GameInfo"] = strGameInfoOut;
  pEsp_->jsonPub["Time2End"] = (String)Time2End;

  serializeJson(pEsp_->jsonPub,strSerialized);
  Serial.printf("Enviando: %s a %s\n-----\n%s\n-----\n", strGameInfo, pEsp_->strTopicGameStatus, strSerialized);
  pEsp_->ptrMqtt->publish(pEsp_->strTopicGameStatus, strSerialized);

}


bool JuegoInfInd::GameRunning() {
	char strAux[100];
	bool bJugando = false;
	static unsigned int msReport = 0;


	// Cambio de estar jugando a inicio de partida (termino la anterior)
	if ((GameStat != STAT_NOMATCH) && (pEsp_->ActiveFace==0)) {
		sprintf(strAux, "Pasando de estado %d a nueva partida", GameStat);
		ReportStatus(strAux);
		GameStat = STAT_NOMATCH;
	}

	// Empezamos configurando
	else if ((GameStat == STAT_NOMATCH) && (pEsp_->ActiveFace!=0)) {
		msMatch_	  = millis();
	    NumFails      = 0;
	    msStart_      = 0;
	    msTries_      = 0;
	    msHold_       = 0;
		msGameHolded_ = 0;
	 	msTryHolded_  = 0; 
	 	TmTotPly 	  = 0;
		TmActive	  = 0;
		TmOnHold	  = 0;
		TmLstTry	  = 0;

		//------ Cambiar estado y reportarlo
	    GameStat      = STAT_WAITSTART;
		ReportStatus((char *)"Inicio partida completado -> wait");
		return false; // salir para no volver a modificar los tiempos
	} 

	// La cara activa ha pasado a ser la del juego => empezamos
	else if ((GameStat == STAT_WAITSTART) && (FaceNumb == pEsp_->ActiveFace)) {		
		Serial.printf("Iniciando juego %d", FaceNumb);
		msStart_ = millis();
		msTries_ = msStart_;
		msHold_   = 0;

		// copiar los parámetros de configuración del juego del último mensaje
		GameTime = pEsp_->LastGameTime;
		FailTime = pEsp_->LastFailTime;
		NumTries = pEsp_->LastNumTries;
		strcpy(GameParm, pEsp_->LastGameParm);

		//------ Cambiar estado y reportarlo
	    GameStat = STAT_PLAYING;
		ReportStatus((char *)"Wait completado -> play");
		msReport = millis();
		return true;
	}
	// {"bugRaro":"abcd","FaceNumb":2,"GameParm":"34","GameTime":300,"FailTime":90,"NumTries":10}

	// Estoy jugando así que reporto compruebo timeout y cada segundo
	else if  ((GameStat == STAT_PLAYING) && (FaceNumb == pEsp_->ActiveFace)) {
		if (millis() > msReport + 1000) {
			msReport = millis();
			if (millis() > 300000) { // TODO PONER LA CONDICION DE TIMEOUT y en otro sitio la del NUMERO MAXIMO DE INTENTOS
			    GameStat = STAT_LOST;
				ReportStatus((char *)"Timeout, has perdido");
			} else { // TODO INDICAR EL TIEMPO QUE QUEDA
				//ReportStatus((char *)"Queda un segundo menos");
			}
		}
	}

	// La cara activa ha dejado de ser la del juego => interrumpimos
	else if  ((GameStat == STAT_PLAYING) && (FaceNumb != pEsp_->ActiveFace)) {
		// copiar la respuesta solución al juego
		strcpy(GameParm, pEsp_->jsonSub["GameParm"]);
		Serial.printf("Parando el juego %d y dando paso a %d", FaceNumb, pEsp_->ActiveFace);
		msHold_ = millis();

		//------ Cambiar estado y reportarlo
	    GameStat = STAT_ONHOLD;
		ReportStatus((char *)"Otro juego -> onhold");
	}

	// La cara estaba parada y vuelve a estar activa => reanudamos
	else if  ((GameStat == STAT_ONHOLD) && (FaceNumb == pEsp_->ActiveFace)) {
		Serial.printf("Reanundadndo el juego %d", FaceNumb);
		msGameHolded_ += (millis()-msHold_);
		msTryHolded_ += (millis()-msHold_);
		msHold_ = 0;

		//------ Cambiar estado y reportarlo
		GameStat = STAT_PLAYING;
		ReportStatus((char *)"Vuelta onhold -> play");
		bJugando = true;
	} 
	TmTotPly = (millis()-msStart_);
	TmOnHold = msGameHolded_;
	TmActive = TmTotPly - TmOnHold;

	return bJugando;
}

/*
    // parámetros de entrada
    int             GameTime;     // Tiempo máximo de juego
    int             FailTime;     // Tiempo máximo hasta fallar
    int             NumTries;     // Número máximo de reintentos
    char            GameParm[30]; // solución del juego o parámetros de entrada
*/