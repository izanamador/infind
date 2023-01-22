#ifndef MQTT_
#define MQTT_

/*
Clase IIMQtt: Gestión del broker MQTT

Requisitos: 
- R05-Dispositivos IoT.1 - Medición de temperatura y humedad
- R05-Dispositivos IoT.1 - Envío periódico de sensores
- R46-Mensajes MQTT.4 Topic: IIX/ESPX/config

- R10-Dispositivos IoT.6 - ChipId
- R12-Aplicación de control y supervisión.2 - Actuación en grupo
- R14-Aplicación de control y supervisión.4 - Conexión a MQTT
- R15-Aplicación de control y supervisión.5 - Datos para conexión
- R39-Dashboard.7 - Alamras de temperatura y humedad
- R43-Mensajes MQTT.1 - Identificadores únicos
- R44-Mensajes MQTT.2 - Conexión y LastWill
- R52-Mensajes MQTT.10 - Desdoblamiento topics de suscripción
- R53-Mensajes MQTT.11 - Orden de campos en nombres de topic

R05-Dispositivos IoT.1 - Medición de temperatura y humedad
  Utilizando el método RefreshDht11, el comunica periódicamente (cada 30 seg) la última medida de temperatura y humedad
  MQTT guarda estos valores y los utiliza en los mensajes periódicos (cada 5 min) 
  El envío del mensaje se realiza en el método PublishDataE que recibe como parámetro immediate par los casos en que no se pueda esperar


R05-Dispositivos IoT.1 - Envío periódico de sensores
  El envío periódico de sensores se implementa en el método PublishDataE que a su vez se invoca desde HasChanged

R10-Dispositivos IoT.6 - ChipId
  Para cumplir este requisito, los dispositivos se conectan a MQTT utilizando el chipId, aunque éste se
  pone al final del nombre del topic para facilitar el uso de máscaras en las suscripciones

R12-Aplicación de control y supervisión.2 - Actuación en grupo
  La actuación en grupo se implementa a partir de los topics que acaban en All:
  - II3/conf/all: configurar todos los dispositivos
  - II3/ledc/all: comandar a todos los leds
  - II3/fotc/all: lanzar las actualizaciones FOTA en todos las placas
  - II3/gamc/all: comandar a todos los juegos

R14-Aplicación de control y supervisión.4 - Conexión a MQTT
  La conexión al servidor MQTT se realiza desde el método IIMqtt::HasChange, que antes de dar paso 
  al método loop de MQTT realiza la conexión o reconexión a MQTT y se suscribe a los topics correspondientes

R15-Aplicación de control y supervisión.5 - Datos para conexión
  Los valores de la dirección IP, puerto, usuario y contraseña para acceder a Mosquito están definidos
  en variables protegidas de la clase para que sean fácilmente mantenibles

R39-Dashboard.7 - Alamras de temperatura y humedad
  Las variables [dht11Tempmin_, dht11Tempmax_, dht11Hummin_, dht11Hummax_] determinan los rangos de temperatura por fuera de los 
  cuales se tiene que enviar una señal de alarma. 
  Estos valores se pueden modificar con el mismo mensaje de configuración que el de la periodicidad deL envío de datos.
  Cada vez que el sensor invoca al método RefreshDht11, se comparan las lecturas de los sensores con los umbrales y se determina si
  se ha producido alguna alarma, especificando de qué sensor y si es por valor demasiado bajo o demasiado alto.
  En el texto de la alarma se incluyen los valores del sensor y del umbral que se ha sobrepasado
  

R43-Mensajes MQTT.1 - Identificadores únicos
  Esta clase implementa la comunicación a través de MQTT incluyendo la utilización de identificadores
  únicos para los topics.
  En el método Setup se inicializa el broker y se generan los nombres de todos los topics para envío y
  recepción, que en algunos casos dependen del identificador del chip

R44-Mensajes MQTT.2 - Conexión y LastWill
  En el método MQTT::HasChanged se realiza la conexión a MQTT añadiendo el mensaje de LastWill con un 
  mensaje retenido.
  Los mensajes online/offline que contienen el chipID se preparan y envían en este método

- R45-Mensajes MQTT.3 Topic: IIX/ESPX/datos

R46-Mensajes MQTT.4 Topic: IIX/ESPX/config
  En el método Setup de la clase IIMqtt se debe indicar la función que recibirá los mensajes
  El prototipo de esta función es: void procesa_mensaje(char* strTopicMsg, byte* payload, unsigned int length)
  Para que los mensajes lleguen al objeto obMqtt, es necesaario añadir una única línea en el cuerpo de la función
    bool mrMqtt = obMqtt.Message(strTopicMsg, payload, length);
  Cuando llega un mensaje por MQTT, la función procesa_mensaje invoca a al método IIMqtt::Message
  Este método es el que realmente hace el trabajo:
  - Reserva memoria y copia el mensaje
  - Formatea el contenido para visualizarlo por el puerto serie
  - Comprueba a qué topic está dirigido y realiza el procesamiento correspondiente
  Para el caso concreto de los mensajes de configuración:
  - Hay dos posibles topics: TopicConfA y TopicConfE, según sea un mensaje genérico (All=Todos) o específico (para un ESP)
  - Los valores de configuración se copian a variables internas: [dataeperiod, dht11Tempmin_, dht11Tempmax_, dht11Hummin_, dht11Hummax_]


- R47-Mensajes MQTT.5 Topic: IIX/ESPX/led/cmd

- R48-Mensajes MQTT.6 Topic: IIX/ESPX/led/status

- R49-Mensajes MQTT.7 Topic: IIX/ESPX/switch/cmd: integrado en led/cmd

- R50-Mensajes MQTT.8 Topic: IIX/ESPX/switch/status: integrado en switch/cmd

- R51-Mensajes MQTT.9 Topic: IIX/ESPX/FOTA


R52-Mensajes MQTT.10 - Desdoblamiento topics de suscripción
  Para permitir ordenar las operaciones de forma individualizada o grupal, cada topic de
  suscripción se ha desdoblado en dos:
  - TopicConfEsp, TopicConfAll: Pedir a un dispositivo o a todos que cambien su configuración
  - TopicLedcEsp, TopicLedcAll: Pedir a un dipositivo o a todos que actualicen el led
  - TopicSwicEsp, TopicSwicAll: No implementado porque se integran en la funcionalidad del Led
  - TopicFotcEsp, TopicFotcAll: Pedir a un dispositivo o a todos una actualización FOTA
  - TopicGamcEsp, TopicGamcAll: Pedir comandos a un juego o a todos



R53-Mensajes MQTT.11 - Orden de campos en nombres de topic

  Para poder utilizar comodines en mensajes enviados y recibidos, se ha modificado el orden
  en que aparecen los campos en los mensajes, dejando el identifador del chip en el último lugar
  Los nombres de topic quedan:
  - II3/conn/esp/<id>				en lugar de IIX/ESPX/conexion 
  - II3/data/esp/<id>   			en lugar de IIX/ESPX/datos
  - II3/conf/all  		  			para configurar a todos los dispositivos
  - II3/conf/esp/<id>   			en lugar de IIX/ESPX/config
  - II3/ledc/all 		   			para comandar a todos los leds
  - II3/ledc/esp/<id>   			en lugar de IIX/ESPX/led/cmd
  - II3/leds/esp/<id>   			en lugar de IIX/ESPX/led/status
  - II3/fotc/all      				para el FOTA de todas los chips
  - II3/fotc/esp/<id>   			en lugar de IIX/ESPX/FOTA
  - II3/gamc/all      				topic adicional para comandar a todos los juegos
  - II3/gamc/1/chrono/ESP_<id>  	topic adicional para comandar al juego
  - II3/gamc/2/numpad/ESP_<id>  	ídem
  - II3/gamc/3/simon/ESP_<id>  		ídem
  - II3/gamc/4/joystick/ESP_<id>  	ídem
  - II3/gamc/5/switch/ESP_<id>  	ídem
  - II3/gamc/6/morse/ESP_<id>  		ídem
  - II3/gams/1/chrono/ESP_<id>  	topic adicional para reportar estado de juego
  - II3/gams/2/numpad/ESP_<id>  	ídem
  - II3/gams/3/simon/ESP_<id>  		ídem
  - II3/gams/4/joystick/ESP_<id>  	ídem
  - II3/gams/5/switch/ESP_<id>  	ídem
  - II3/gams/6/morse/ESP_<id>  		ídem


*/

#include "IIWifi.h"
#include <PubSubClient.h>  // MQTT
#include "ArduinoJson.h"   // JSON

class IIMqtt {
public:
	// Métodos
	void Setup(
			IIWifi *ptWifi,
			void callback (char* strTopicMsg, byte* payload, unsigned int length)
		);	
	bool HasChanged();   				// Cada iteración hay que comprobar si llegaron mensajes
	void Subscribe(char* strTopic);
  bool Message(char* strTopic, byte* payload, unsigned int length);
  void RefreshDht11(int pinSda, float fltTemp, float fltHum);
  void PublishDataE(bool immediate=false);

  // Parámetros de configuración recibidos y valores por defecto
  int     dataeperiod = 300;     // inicialmente los mensajes se envían cada 5 minutos
  

	// Propiedades
	PubSubClient MClient; 
	const int   MQTT_MAX_BUFFER = 512;
	char TopicConnE[50];  // II3/conn/esp/<id>
	char TopicDataE[50];  // II3/conn/esp/<id>			
	char TopicConfA[50];  // "II3/conf/all";
	char TopicConfE[50];  // II3/conf/esp/<id>
	char TopicLedcA[50];  // "II3/ledc/all";
	char TopicLedcE[50];  // II3/ledc/esp/<id>
	char TopicLedsE[50];  // II3/leds/esp/<id>   			
	char TopicFotcA[50];  // "II3/fotc/all";
	char TopicFotcE[50];  // II3/fotc/esp/<id>   			
	char TopicGamcA[50];  // "II3/gamc/all";
	char TopicGamc1[50];  // II3/gamc/1/chrono/ESP_<id>
	char TopicGamc2[50];  // II3/gamc/2/numpad/ESP_<id>
	char TopicGamc3[50];  // II3/gamc/3/simon/ESP_<id>
	char TopicGamc4[50];  // II3/gamc/4/joystick/ESP_<id>
	char TopicGamc5[50];  // II3/gamc/5/switch/ESP_<id>
	char TopicGamc6[50];  // II3/gamc/6/morse/ESP_<id>
	char TopicGams1[50];  // II3/gams/1/chrono/ESP_<id>
	char TopicGams2[50];  // II3/gams/2/numpad/ESP_<id>
	char TopicGams3[50];  // II3/gams/3/simon/ESP_<id>
	char TopicGams4[50];  // II3/gams/4/joystick/ESP_<id>
	char TopicGams5[50];  // II3/gams/5/switch/ESP_<id>
	char TopicGams6[50];  // II3/gams/6/morse/ESP_<id>
	void Subsribe(char *strTopic);

private:
  IIWifi *ptWifi_; 
 	const char* MQTT_SERVER     = "iot.ac.uma.es";
	const char* MQTT_USER       = "II3"; // "infind";
	const char* MQTT_PASS       = "qW30SImD"; // "zancudo";  	
	char  espid_[15];


  // almacena la última medición del dht11 para cuando toque enviar
  int   dht11Pinsda_;    
  float dht11Temp_;  
  float dht11Hum_;  
  char  dht11AlarmTemp_[40];
  char  dht11AlarmHum_ [40];
  float dht11Tempmin_ = 15;
  float dht11Tempmax_ = 25;
  float dht11Hummin_  = 45;
  float dht11Hummax_  = 55;
};


// PubSubClient Mqtt_client(ptWifi.Client);         // cliente mqtt
#endif