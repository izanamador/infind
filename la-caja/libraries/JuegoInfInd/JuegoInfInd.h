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
    // parámetros de entrada
    char            gameparam[30]; // solución del juego o parámetros de entrada
    int             maxtime;       // Tiempo máximo de juego
    int             maxlives;      // Número máximo de reintentos
    // parámetros de entrada/salida
    int             activeface;    // Cara activa en cada momento
    int             facenumb;      // número de la cara del cubo
    char            gamename[20];  // numpad, simon, joystick, switch, barco
    char            gameinfo[200]; // información adicional sobre el 

    // parámetros de salida
    int             gamestate;     // estado actual del juego
    int             remtime;       // tiempo restante en segundos
    int             remlives;      // vidas que quedan por gastar
    //int             TmTotPly;
    //int             TmActive;
    //int             TmOnHold;
    //int             TmLstTry;
    
    //int             FailTime;     // Tiempo máximo hasta fallar
    //char            Time2End[20]; // 15:00

    
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


