#include "Arduino.h"
#include "JuegoInfInd.h"


char strGameInfoOut[100];
//#define STAT_TIMEOUT  6
//#define STAT_END      7


JuegoInfInd::JuegoInfInd(const char *gameName, int faceNumb, EspInfInd *pEspInfInd) {

    //------- Inicialización de variables del objeto
		pEsp_ = pEspInfInd; //---  puntero al ESP donde está implementado el juego
		activeface = 0;
    	gamestate = STAT_NEWMATCH;
		strcpy(gameparam, "");
		maxtime = 0;
		maxlives = 0;

    //--- registro de la configuración del juego
		facenumb = faceNumb;
		strcpy(gamename, gameName);
		//sprintf(strGameInfoOut, "Game name: %s", GameName);

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

	activeface = pEsp_->ActiveFace; 

	if (stLast != gamestate) {Serial.printf("%d->%d\n", stLast, gamestate);stLast=gamestate;}

	// esperar a que empiece una partida
	if ((gamestate== STAT_NEWMATCH) && (activeface==0))
		;

	// todos los juegos inicializan variables, reportan a NR y pasan a esperar su turno
	else if ((gamestate== STAT_NEWMATCH) && (activeface!=0)) {
		pEsp_->Debug(1, "newmatch y active face > 0");
		msMatch_	  = millis();
	    //NumFails      = 0;
	    msStart_      = 0;
	    msTries_      = 0;
	    msHold_       = 0;
		msGameHolded_ = 0;
	 	msTryHolded_  = 0; 
	 	//TmTotPly 	  = 0;
		//TmActive	  = 0;
		//TmOnHold	  = 0;
		//TmLstTry	  = 0;

		//------ Cambiar estado y reportarlo
	    gamestate     = STAT_WAITSTART;
		ReportStatus((char *)"NewMatch -> WaitStart");
		// return false; // salir para no volver a modificar los tiempos
	} 

	// en mitad de una partida se pide empezar otra, todos los juegos empiezan de nuevo
	else if ((gamestate!= STAT_NEWMATCH) && (activeface==0)) {
		pEsp_->Debug(2, "newmatch y active face > 0");
		sprintf(strAux, "Pasando de estado %d a nueva partida", gamestate);
		ReportStatus(strAux);
		gamestate= STAT_NEWMATCH;
	}

	// esperar a que empiece el juego
	else if ((gamestate== STAT_WAITSTART) && (facenumb != activeface))
		;

	// el juego se activó: resetear tiempos y copiar parámetros específicos del juego
	else if ((gamestate== STAT_WAITSTART) && (facenumb == activeface)) {
		//--- resetear temporizadores
			pEsp_->Debug(4, "Iniciando el juego");
			Serial.printf("Iniciando juego %d", facenumb);
			msStart_ = millis();
			msTries_ = msStart_;
			msHold_   = 0;

		//- copiar los parámetros específicos que se guardaron cuando llegó el último mensaje
			maxtime 	= 		pEsp_->maxtimeLast;
			maxlives 	= 		pEsp_->maxlivesLast;
			strcpy(gameparam, 	pEsp_->gameparamLast);
			strcpy(gameinfo, 	pEsp_->gameinfoLast);			
			//NumTries = pEsp_->LastNumTries; // TODO EN LUGAR DE NUMTRIES SERÍA MAXSETS NUMSETS

		//- inicializar el tiempo y número de vidas
			remtime 	= maxtime;
			remlives 	= maxlives;

		//------ Cambiar estado para que el juego pueda inicializarse y reportarlo
		    gamestate= STAT_NEWGAME;
			pEsp_->Debug(5, "Inicio genérico completado -> nuevo intento");
			// msReport = millis();
			return true; // el juego específico debería inicializar el juego (melodía simón)
	}

	// nuevo set => el juego recibe el control para inicializar  debería inicializar lo que necesite
	else if ((gamestate== STAT_NEWGAME) && (facenumb == activeface)) {		
		pEsp_->Debug(5, "Iniciando el set");
	    gamestate= STAT_NEWSET;
	    return true;
	}	

	// nuevo set => el juego recibe el control para inicializar  debería inicializar lo que necesite
	else if ((gamestate== STAT_NEWSET) && (facenumb == activeface)) {
		pEsp_->Debug(5, "Inicio específico completado -> nuevo intento");
	    gamestate= STAT_PLAYING;
		msReport = millis();
		return true; // el juego específico debería incializar el intento (4 notas)
	}


	// Estoy jugando así que reporto compruebo timeout y cada segundo
	else if  ((gamestate== STAT_PLAYING) && (facenumb == activeface)) {
		if (millis() > msReport + 5000) { // TODO QUE EL TIEMPO SEA CONFIGURABLE
			msReport = millis();
			if (millis() > 300000) { // TODO PONER LA CONDICION DE TIMEOUT y en otro sitio la del NUMERO MAXIMO DE INTENTOS
			    gamestate= STAT_LOST;
				ReportStatus((char *)"Timeout, has perdido");
			} else { // TODO INDICAR EL TIEMPO QUE QUEDA Y QUE LA PERIODICIDAD SEA CONFIGURABLE
				ReportStatus((char *)"Quedan 300 un segundos menos");
			}
		}
		return true; // el juego recibe el control
	}

	// La cara activa ha dejado de ser la del juego => Cambiar estado y reportarlo
	else if  ((gamestate== STAT_PLAYING) && (facenumb != activeface)) {
		msHold_ = millis();

		sprintf(gameinfo, "%d: paro para dar paso a %d", facenumb, activeface);
	    gamestate= STAT_ONHOLD;
		ReportStatus(gameinfo);

		//pEsp_->Debug(6, "Otro juego => onhold");
		// copiar la respuesta solución al juego
		//!strcpy(GameParm, pEsp_->jsonSub["GameParm"]);
	}

	// La cara estaba parada y vuelve a estar activa => reanudamos
	else if  ((gamestate== STAT_ONHOLD) && (facenumb == activeface)) {
		msGameHolded_ += (millis()-msHold_);
		msTryHolded_ += (millis()-msHold_);
		msHold_ = 0;

		sprintf(gameinfo, "%d: reanudo después de parar", facenumb);
		gamestate= STAT_PLAYING;
		ReportStatus((char *)"Vuelta onhold -> play");
		bJugando = true;

	    //Serial.printf("Reanundadndo el juego %d", facenumb);
	} 
	//TmTotPly = (millis()-msStart_);
	//TmOnHold = msGameHolded_;
	//TmActive = TmTotPly - TmOnHold;

	return false; // el juego específico no recibe el control
}

#define REPORT_MSG_RETRY "%d minutes and %d seconds remainig. You have %d lives left. Try again"
#define REPORT_MSG_LOST  "%d minutes and %d seconds remainig. You have %d lives left. You lost this game, try another one"
#define REPORT_MSG_WON   "You won. Play next game"

void JuegoInfInd::ReportSuccess(char* strGameInfo){
	sprintf(gameinfo, REPORT_MSG_WON);
	gamestate= STAT_WON;
	ReportStatus(strGameInfo);
}


void JuegoInfInd::ReportFail(char* strGameInfo){
    remlives--;
    int remMinutes = remtime/60;
    int remSeconds = remtime%60;    

    if (remlives > 0) {
    	sprintf(gameinfo, REPORT_MSG_RETRY, remMinutes, remSeconds, remlives);
    	gamestate= STAT_NEWSET; // volver a intentarlo de nuevo
    } 
    else {
    	sprintf(gameinfo, REPORT_MSG_LOST, remMinutes, remSeconds, remlives);
    	gamestate= STAT_LOST; // volver a intentarlo de nuevo	
    }
	// TmLstTry = millis()-msTries_-msTryHolded_;

	ReportStatus(strGameInfoOut);

    msTries_ = millis();
    msTryHolded_ = 0;
}



void JuegoInfInd::ReportStatus (char *strGameInfo) {

	//- Cabecera del mensaje común
		pEsp_->Debug(1, "inicializa el mensaje con los valores por defecto del ESP");
	  	pEsp_->MqttFormatMsg(TopicPub, strGameInfo, OPT_MSG_FORMAT);

	//- datos epsecíficos del juego
		pEsp_->Debug(2, "prepara los datos específicos del juego");
	  	pEsp_->jsonPub["gameinfo"] 		= gameinfo;
	  	pEsp_->jsonPub["maxlives"] 		= maxlives;
	  	pEsp_->jsonPub["remlives"] 		= remlives;
	  	pEsp_->jsonPub["maxtime"] 		= maxtime;
	  	pEsp_->jsonPub["remtime"] 		= remtime;
	  	pEsp_->jsonPub["gamestate"] 	= gamestate;
	  	pEsp_->jsonPub["facenumb"] 		= facenumb;
	  	pEsp_->jsonPub["activeface"] 	= activeface;
	  	pEsp_->jsonPub["gamename"] 		= gamename;
	  	pEsp_->jsonPub["gameparam"] 	= gameparam;

	//- Serialización y envío
		pEsp_->Debug(3, "serializa el json");
	  	char strSerialized[JSON_MESSAGE_SIZE];
	  	serializeJson(pEsp_->jsonPub,strSerialized);  	
	  	Serial.printf("Juego enviando: %s a %s\n-----\n%s\n-----\n", strGameInfo, pEsp_->strTopicGameStatus, strSerialized);
	  	pEsp_->Debug(4, "envía los datos por MQTT");
	  	pEsp_->ptrMqtt->publish(pEsp_->strTopicGameStatus, strSerialized);
}


