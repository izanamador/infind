#include "Arduino.h"
#include "JuegoInfInd.h"


char strGameInfoOut[100];
//#define STAT_TIMEOUT  6
//#define STAT_END      7


JuegoInfInd::JuegoInfInd(const char *gameName, int faceNumb, EspInfInd *pEspInfInd) {

    //------- Inicialización de variables del objeto
		pEsp_ = pEspInfInd; //---  puntero al ESP donde está implementado el juego
		activeface = 0;
		acttime = 0;
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



#define REPORT_MSG_STARTGAME "%s starting, you have %d lives %d minutes and %d seconds to solve it"
#define REPORT_MSG_TIMEOUT   "Timeout. You lost %s but can try another game"

bool JuegoInfInd::GameRunning() {
	char strAux[100];
	bool bJugando = false;
	static unsigned int msReport = 0;
	static int stLast = -1;

	activeface = pEsp_->ActiveFace; 
	//- si se está jugando mi juego, refrescar los parámetros de configuración y temporizadores
 	if (activeface == facenumb) {
	 		//- Refrescar parámetros
		 		maxtime 		= pEsp_->maxtimeLast;
		 		maxlives 		= pEsp_->maxlivesLast;
				strcpy(gameparam, pEsp_->gameparamLast);
				strcpy(gameinfo , pEsp_->gameinfoLast);

			//- Actualizar temporizadores
				acttime 		= (int)((millis()-msStart_-msWaiting_)/1000);
				remtime 		= maxtime-acttime;
				//activefaceLast      = jsonSub["activeface"].as<int>();
	 	}

	//- esperar a que empiece una partida
	if ((gamestate== STAT_NEWMATCH) && (activeface==0))
			;

	//- si hay una nueva partida cada juego debe reinicializar
	else if ((gamestate == STAT_NEWMATCH) && (activeface!=0)) {		
			pEsp_->Debug(30, "newmatch y active face > 0");
			msMatch_	  = millis();
    		gamestate     = STAT_WAITSTART;
	 		// msTryHolded_  = 0; 
			// msGameHolded_ = 0;
			// ReportStatus((char *)"NewMatch -> WaitStart");
	    	// NumFails      = 0;
	    	// msStart_      = 0;
	    	// msTries_      = 0;
	    	// msHold_       = 0;		
		}

	//- en mitad de una partida se pide empezar otra => enviar el último estado
	else if (((gamestate==STAT_PLAYING) || (gamestate==STAT_ONHOLD)) && (activeface==0)) {
			sprintf(gameinfo, "%s aborted", gamename);
			ReportStatus(gameinfo);
			gamestate= STAT_NEWMATCH;
		}

	//- la partida ha empezado pero hay que esperar a que se active el juego
	else if ((gamestate== STAT_WAITSTART) && (facenumb != activeface))
		;

	//- el juego se activó por primera vez: resetear temporizadores y reportar inicio
	else if ((gamestate== STAT_WAITSTART) && (facenumb == activeface)) {
		//--- resetear temporizadores
			gamestate= STAT_NEWGAME;
			sprintf(gameinfo, REPORT_MSG_STARTGAME, gamename, maxlives, maxtime/60, maxtime%60);
			ReportStatus(gameinfo);
			acttime 	= 0;
			remtime 	= maxtime;
			remlives 	= maxlives;
			msStart_ 	= millis();
			msHold_   	= 0;
			msWaiting_  = 0;
			return true; // el juego específico debería inicializar el juego (melodía simón)

			// //- copiar los parámetros específicos que se guardaron cuando llegó el último mensaje
				//msTries_ 	= msStart_;
				// 	maxtime 	= 		pEsp_->maxtimeLast;
				// 	maxlives 	= 		pEsp_->maxlivesLast;
				// 	strcpy(gameparam, 	pEsp_->gameparamLast);
				// 	strcpy(gameinfo, 	pEsp_->gameinfoLast);			
				// 	//NumTries = pEsp_->LastNumTries; // TODO EN LUGAR DE NUMTRIES SERÍA MAXSETS NUMSETS
				//- inicializar el tiempo y número de vidas
		}

	//- nueva partida => el juego recibe el control para inicializar lo que necesite
	else if ((gamestate== STAT_NEWGAME) && (facenumb == activeface)) {		
			pEsp_->Debug(5, "Iniciando el set");
		    gamestate= STAT_NEWSET;
		    return true;
		}	

	//- nueva vida => el juego recibe el control para inicializar lo que necesite
	else if ((gamestate== STAT_NEWSET) && (facenumb == activeface)) {
			pEsp_->Debug(5, "Inicio específico completado -> nuevo intento");
		    gamestate= STAT_PLAYING;
			//msReport = millis();
			return true; // el juego específico debería incializar el intento (4 notas)
		}

	//- si se acabe el tiempo durante el juego hay que parar y pasar el control al crono
	else if  ((gamestate== STAT_PLAYING) && (facenumb == activeface) && (remtime <=0)) {
			sprintf(gameinfo, REPORT_MSG_TIMEOUT, gamename);
		    gamestate= STAT_LOST;
			ReportStatus(gameinfo);
		}

	// TODO QUE EL TIEMPO SEA CONFIGURABLE
	//- durante el juego hay que informar periódicamente y detectar si se acabó el tiempo
	else if  ((gamestate== STAT_PLAYING) && (facenumb == activeface) && millis()>msReport+5000) {
			msReport = millis();
			remtime = maxtime - (int)((msStart_-msWaiting_)/1000);
			if (remtime <= 10) {
				sprintf(gameinfo, "%d", remtime);
			}
			else if (remtime%60 == 0) {
				sprintf(gameinfo, "%d minutes left", remtime/60);
			}
			ReportStatus(gameinfo);			
			return true; // el juego recibe el control
		}

	//- La cara deja de estar activa => Cambiar estado y reportarlo
	else if ((gamestate== STAT_PLAYING) && (facenumb != activeface)) {
			sprintf(gameinfo, "%d: espera y da paso a %d", facenumb, activeface);
			msHold_ = millis();
		    gamestate= STAT_ONHOLD;
			ReportStatus(gameinfo);
			//pEsp_->Debug(6, "Otro juego => onhold");
			// copiar la respuesta solución al juego
			//!strcpy(GameParm, pEsp_->jsonSub["GameParm"]);
		}

	// La cara estaba parada y vuelve a estar activa => reanudamos
	else if ((gamestate== STAT_ONHOLD) && (facenumb == activeface)) {
		msWaiting_ += (millis()-msHold_);
		msHold_ = 0;
		//msTryHolded_ += (millis()-msHold_);

		sprintf(gameinfo, "resuming %s", gamename);
		gamestate= STAT_PLAYING;
		ReportStatus(gameinfo);
		//return = true;
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
    //msTryHolded_ = 0;
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


