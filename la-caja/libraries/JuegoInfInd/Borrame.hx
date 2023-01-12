#ifndef JUEGOINFIND_
#define JUEGOINFIND_

#include "EspInfInd.h"

class JuegoInfInd
{
  public:
    JuegoInfInd(const char *nombreJuego, int numCara, EspInfInd *pEspInfInd);
    ~JuegoInfInd();
    void            Setup();
    bool            GameRunning();
    void            Start  (char* strGameInfo);
    void            Status (char* strGameInfo);
    void            Failure(char* strGameInfo);
    void            Success(char* strGameInfo);
    char            GameSolution[30]; // solución del juego
    char            TopicPub[TOPIC_NAME_MAX];
    char            TopicSub[TOPIC_NAME_MAX];
    char            Nombre[20]; // numpad, simon, joystick, switch, barco
    int             NumCara;
    int             TmTotal;
    int             TmActivo;
    int             TmParado;
    int             TmIntento;
    int             currStat_;  // estado actual del juego
    
  private:
    EspInfInd       *pEsp_;     // puntero al objeto que gestiona el chip
    int             numTries_;  // número de reintentos de juego
    unsigned int    msStart_;  // milisegundos desde el inicio del juego
    unsigned int    msTries_;  // milisegundos desde el último intento
    unsigned int    msHold_;   // milisegundos en que se detuvo el crono
    unsigned int    msGameHolded_;  // milisegundos que el juego ha estado parado
    unsigned int    msTryHolded_;   // milisegundos que el intento ha estado parado
};

#endif


