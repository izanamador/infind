#include "Arduino.h"
#include "JuegoInfInd.h"


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
    GameStat = STAT_NEWMATCH;
	strcpy(GameParm, "");
	strcpy(TopicPub, pEsp_->strTopicGameStatus);
}



JuegoInfInd::~JuegoInfInd() {
  ;
}



void JuegoInfInd::Setup() {
}


bool JuegoInfInd::GameRunning() {
	char strAux[100];
	bool bJugando = false;
	static unsigned int msReport = 0;
	static int stLast = -1;
	if (stLast != GameStat) {Serial.printf("%d->%d\n", stLast, GameStat);stLast=GameStat;}

	// esperar a que empiece una partida
	if ((GameStat == STAT_NEWMATCH) && (pEsp_->ActiveFace==0))
		;

	// todos los juegos inicializan variables, reportan a NR y pasan a esperar su turno
	else if ((GameStat == STAT_NEWMATCH) && (pEsp_->ActiveFace!=0)) {
		pEsp_->Debug(1, "newmatch y active face > 0");
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
		ReportStatus((char *)"NewMatch -> WaitStart");
		// return false; // salir para no volver a modificar los tiempos
	} 

	// en mitad de una partida se pide empezar otra, todos los juegos empiezan de nuevo
	else if ((GameStat != STAT_NEWMATCH) && (pEsp_->ActiveFace==0)) {
		pEsp_->Debug(2, "newmatch y active face > 0");
		sprintf(strAux, "Pasando de estado %d a nueva partida", GameStat);
		ReportStatus(strAux);
		GameStat = STAT_NEWMATCH;
	}

	// esperar a que empiece el juego
	else if ((GameStat == STAT_WAITSTART) && (FaceNumb != pEsp_->ActiveFace))
		;

	// el juego se activó: resetear tiempos y copiar parámetros específicos del juego
	else if ((GameStat == STAT_WAITSTART) && (FaceNumb == pEsp_->ActiveFace)) {		
		pEsp_->Debug(4, "Iniciando el juego");
		Serial.printf("Iniciando juego %d", FaceNumb);
		msStart_ = millis();
		msTries_ = msStart_;
		msHold_   = 0;

		// copiar los parámetros específicos que se guardaron cuando llegó el último mensaje
		GameTime = pEsp_->LastGameTime;
		FailTime = pEsp_->LastFailTime;
		NumTries = pEsp_->LastNumTries; // TODO EN LUGAR DE NUMTRIES SERÍA MAXSETS NUMSETS
		strcpy(GameParm, pEsp_->LastGameParm);

		//------ Cambiar estado para que el juego pueda inicializarse y reportarlo
	    GameStat = STAT_NEWGAME;
		ReportStatus((char *)"Inicio genérico completado -> nuevo intento");
		msReport = millis();
		return true; // el juego específico debería inicializar el juego (melodía simón)
	}

	// nuevo set => el juego recibe el control para inicializar  debería inicializar lo que necesite
	else if ((GameStat == STAT_NEWGAME) && (FaceNumb == pEsp_->ActiveFace)) {		
		pEsp_->Debug(5, "Iniciando el set");
	    GameStat = STAT_NEWSET;
	    return true;
	}	

	// nuevo set => el juego recibe el control para inicializar  debería inicializar lo que necesite
	else if ((GameStat == STAT_NEWSET) && (FaceNumb == pEsp_->ActiveFace)) {
		pEsp_->Debug(5, "Inicio específico completado -> nuevo intento");
	    GameStat = STAT_PLAYING;
		msReport = millis();
		return true; // el juego específico debería incializar el intento (4 notas)
	}


	// Estoy jugando así que reporto compruebo timeout y cada segundo
	else if  ((GameStat == STAT_PLAYING) && (FaceNumb == pEsp_->ActiveFace)) {
		if (millis() > msReport + 5000) { // TODO QUE EL TIEMPO SEA CONFIGURABLE
			msReport = millis();
			if (millis() > 300000) { // TODO PONER LA CONDICION DE TIMEOUT y en otro sitio la del NUMERO MAXIMO DE INTENTOS
			    GameStat = STAT_LOST;
				ReportStatus((char *)"Timeout, has perdido");
			} else { // TODO INDICAR EL TIEMPO QUE QUEDA Y QUE LA PERIODICIDAD SEA CONFIGURABLE
				ReportStatus((char *)"Quedan 300 un segundos menos");
			}
		}
		return true; // el juego recibe el control
	}

	// La cara activa ha dejado de ser la del juego => interrumpimos
	else if  ((GameStat == STAT_PLAYING) && (FaceNumb != pEsp_->ActiveFace)) {
		pEsp_->Debug(6, "Guardar una copia de la respuesta");
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

	return false; // el juego específico no recibe el control
}

void JuegoInfInd::ReportFail(char* strGameInfo){
    NumFails++;
    strcpy(strGameInfoOut, strGameInfo);
	TmLstTry = millis()-msTries_-msTryHolded_;
	GameStat = STAT_LOST; // indicar a NR que se falló
	ReportStatus(strGameInfoOut);

    msTries_ = millis();
    msTryHolded_ = 0;
    GameStat = STAT_NEWSET; // volver a intentarlo de nuevo
}

void JuegoInfInd::ReportSuccess(char* strGameInfo){
	sprintf(strGameInfoOut, "Has gandado %s.\n%s", GameName,strGameInfo);
	GameStat = STAT_WON;
	ReportStatus(strGameInfoOut);
}


void JuegoInfInd::ReportStatus (char *strGameInfo) {

	// 
	pEsp_->Debug(1, "inicializa el mensaje con los valores por defecto del ESP");
  	pEsp_->MqttFormatMsg(pEsp_->strTopicGameStatus, strGameInfo, OPT_MSG_FORMAT);

	pEsp_->Debug(2, "prepara los datos específicos del juego");
  	pEsp_->jsonPub["gamestate"] = GameStat;

	pEsp_->Debug(3, "serializa el json");
  	char strSerialized[JSON_MESSAGE_SIZE];
  	serializeJson(pEsp_->jsonPub,strSerialized);  	
  	Serial.printf("Juego enviando: %s a %s\n-----\n%s\n-----\n", strGameInfo, pEsp_->strTopicGameStatus, strSerialized);

  	pEsp_->Debug(4, "envía los datos por MQTT");
  	pEsp_->ptrMqtt->publish(pEsp_->strTopicGameStatus, strSerialized);



//  int secs = GameTime - TmActive/1000; // segundos restantes
//  sprintf(Time2End, "--%02d:%02d--", secs/60, secs%60);
//  
//  Serial.printf("\nTime2End %s\n", Time2End);
//
//  //------------ Parámetros de salida
//  pEsp_->jsonPub["NumFails"] = NumFails;
//  pEsp_->jsonPub["TmTotPly"] = TmTotPly;
//  pEsp_->jsonPub["TmActive"] = TmActive;
//  pEsp_->jsonPub["TmOnHold"] = TmOnHold;
//  pEsp_->jsonPub["TmLstTry"] = TmLstTry;
//  pEsp_->jsonPub["GameInfo"] = strGameInfoOut;
//  pEsp_->jsonPub["Time2End"] = (String)Time2End;
//
//  pEsp_->jsonPub["Origin"] = STR_ORG_BOARD;
//  pEsp_->jsonPub["MqttId"] = "";
//  pEsp_->jsonPub["UpTime"] = millis();
//
//  //------------ Datos specíficos del estado del juego
//  pEsp_->jsonPub["FaceNumb"] = FaceNumb;
//  pEsp_->jsonPub["GameName"] = GameName;
//
//  //------------ Parámetros de entrada
//  pEsp_->jsonPub["GameTime"] = GameTime;
//  pEsp_->jsonPub["FailTime"] = FailTime;
//  pEsp_->jsonPub["NumTries"] = NumTries;
//  pEsp_->jsonPub["GameParm"] = GameParm;


}


