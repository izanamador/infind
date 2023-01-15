#ifndef JUEGOINFIND_
#define JUEGOINFIND_

#include "EspInfInd.h"

#define STAT_NEWMATCH     0 // BLANCO
#define STAT_WAITSTART    1 // AZUL
#define STAT_NEWGAME      11 // transitorio el juego genérico se inicaliza
#define STAT_NEWSET       12 // transitorio el juego específico se inicializa
#define STAT_PLAYING      2 // VERDE
#define STAT_ONHOLD       3 // ROJO
#define STAT_LOST         4 // NEGRO
#define STAT_WON          5 // VIOLETA

class JuegoInfInd
{
  public:
    JuegoInfInd(const char *gameName, int faceNumb, EspInfInd *pEspInfInd);
    ~JuegoInfInd();
    void            Setup();

    bool            GameRunning();
    void            ReportStatus (char* strGameInfo);
    void            ReportSuccess(char* strGameInfo);
//    void            ReportStart(char* GameInfo);
//    void            ReportStatus(char* GameInfo);
    void            ReportFail(char* strGameInfo);
//    void            ReportSuccess(char* GameInfo);
    char            TopicPub[TOPIC_NAME_MAX];
    char            TopicSub[TOPIC_NAME_MAX];

    // parametros del juego
    int             FaceNumb;     // número de la cara del cubo
    char            GameName[20]; // numpad, simon, joystick, switch, barco
    // parámetros de entrada
    int             GameTime;     // Tiempo máximo de juego
    int             FailTime;     // Tiempo máximo hasta fallar
    int             NumTries;     // Número máximo de reintentos
    char            GameParm[30]; // solución del juego o parámetros de entrada
    char            Time2End[20]; // 15:00

    // parámetros de salida
    int             GameStat;     // estado actual del juego
    int             NumFails;     // número de reintentos de juego
    int             TmTotPly;
    int             TmActive;
    int             TmOnHold;
    int             TmLstTry;
    
  private:
    EspInfInd       *pEsp_;     // puntero al objeto que gestiona el chip
    unsigned int    msMatch_;  // milisegundos desde el inicio de la partida
    unsigned int    msStart_;  // milisegundos desde el inicio del juego
    unsigned int    msTries_;  // milisegundos desde el último intento
    unsigned int    msHold_;   // milisegundos en que se detuvo el crono
    unsigned int    msGameHolded_;  // milisegundos que el juego ha estado parado
    unsigned int    msTryHolded_;   // milisegundos que el intento ha estado parado
};

#endif


