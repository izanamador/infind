/*
Clase IIDht11: Gestión del sensor de temperatura y humedad

Requisitos: 
- R05-Dispositivos IoT.1 - Medición de temperatura y humedad
- R45-Mensajes MQTT.3 - Envío de temperatura y humedad


R05-Dispositivos IoT.1 - Medición de temperatura y humedad

  El método HasChanged consulta periódicamente el sensor de temperatura y humedad
  Aunque la especificación indica que las mediciones deben enviarse cada 5 minutos,
  la periodicidad de la medición es mucho menor (30 segundos), para que la alarma 
  que se envía cuando se supera la temperatura no tenga tanto retardo

  Como no todas las placas tendrán un sensor instalado, se puede invocar a Setup con 
  un número negativo para indicar este hecho, con lo que bastaría con poner un signo menos
  en el código de la aplicación para que el código del Dht11 inexistente no interfiera
  con el resto de la aplicaciuón

  Cuando se termina de medir el sensor el método HasChanged invoca a ptMqtt->RefreshDht11
  para comunicar la última medición que se utilicen esos valores en el próximo envío periódico
  
R45-Mensajes MQTT.3 - Envío de temperatura y humedad

  El sensor Dht11 envía periódicamente los datos de temperatura y humedad a la clase IIMqtt
  La periodicidad de lectura de los sensores es menor que la de envío de datos, por lo que 
  parte de los valores medidos se descartan. El motivo de la diferencia de periodicidad es
  dar la posibilidad de alarma antes de que acabe el periodo de envío de datos.

  

*/

#ifndef IIDHT11_
#define IIDHT11_
#include "DHTesp.h"        // DHT11
#include "IIHardware.h"
#include "IIMqtt.h"
//#define D1_GPIO05_SDA       5

class IIDht11 {
public:
    void Setup(IIMqtt *ptMqtt, int pinSda = D1_GPIO05_SDA, unsigned int statPeriod=30);
    bool HasChanged(void);

  	// getters y setters
    int   getSda();
    float getTemperature();
    float getHumidity();
      
private:
    IIMqtt *ptMqtt_;                // puntero a Mqtt para guardar los sensores después de cada medida
    DHTesp dht_;                    // sensor de temperatura DHT11
    unsigned int statPeriod_;       // periodo de publicación de mensajes en segundos
    unsigned int msLastStat_;       // última vez que se publicó un mensaje

    int   pinsda_;                  // pin al que está conectado el sensor
    float fltTemp_; 			    // última lectura de temperatura
    float fltHum_;			        // última lectura de humedad
};

// PubSubClient Mqtt_client(ptWifi.Client);         // cliente mqtt
#endif