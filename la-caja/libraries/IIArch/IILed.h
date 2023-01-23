/*
Clase IILed: Gestión del led

Requisitos: 
- R05-Dispositivos IoT.1 - Medición de temperatura y humedad


R05-Dispositivos IoT.1 - Medición de temperatura y humedad

  El método HasChanged consulta periódicamente el sensor de temperatura y humedad
  Aunque la especificación indica que las mediciones deben enviarse cada 5 minutos,
  la periodicidad de la medición es mucho menor (30 segundos), para que la alarma 
  que se envía cuando se supera la temperatura no tenga tanto retardo


*/

#ifndef IILED_
#define IILED_
#include "IIHardware.h"
#include "IIMqtt.h"
//#define D1_GPIO05_SDA       5

class IILed {
public:
    void Setup(int pinled);
    // Setup(IIMqtt *ptMqtt, int pinSda = D1_GPIO05_SDA, unsigned int statPeriod=30);
    bool HasChanged(void);
    int  PctToLight(float percent);
    void SetLight(bool lighted=true, float maxlevel=0.5);
      
private:
    int    pinled_;    // pin al que está conectado el sensor
    bool   lighted_;   // encendido o no
    int    maxlevel_;  // porcentaje al que tiene que bajar la luminosidad

};

#endif