#ifndef MQTT_
#define MQTT_

/*
Clase IIWifi: Conexión a la WIFI

Requisitos: 
- R10-Dispositivos IoT.6 - ChipId
- R12-Aplicación de control y supervisión.2 - Actuación en grupo
- R14-Aplicación de control y supervisión.4 - Conexión a MQTT
- R15-Aplicación de control y supervisión.5 - Datos para conexión
- R43-Mensajes MQTT.1 - Identificadores únicos
- R44-Mensajes MQTT.2 - Conexión y LastWill
- R52-Mensajes MQTT.10 - Desdoblamiento topics de suscripción
- R53-Mensajes MQTT.11 - Orden de campos en nombres de topic

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

- R46-Mensajes MQTT.4 Topic: IIX/ESPX/config

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

class IIMqtt {
public:
	// Métodos
	void Setup(
			IIWifi *ptWifi,
			void callback (char* strTopicMsg, byte* payload, unsigned int length)
		);	
	bool HasChanged();   				// Cada iteración hay que comprobar si llegaron mensajes
	void Subscribe(char* strTopic);

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
 	const char* MQTT_SERVER     = "iot.ac.uma.es";
  	const char* MQTT_USER       = "II3"; // "infind";
  	const char* MQTT_PASS       = "qW30SImD"; // "zancudo";  	
  	char  espid_[15];

};

// PubSubClient Mqtt_client(ptWifi.Client);         // cliente mqtt
#endif