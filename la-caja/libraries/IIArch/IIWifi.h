#ifndef IIWIFI_
#define IIWIFI_

/*
Clase IIWifi: Conexión a la WIFI

Requisitos: 
- R45-Mensajes MQTT.3: Los mensajes de datos contienen los parámetros de la red que hay que enviar por MQTT
	"Wifi":{"SSId":"infind","IP":"192.168.0.100","RSSI":56}

Funcionalidad:
- La conexión se realiza en el método setup
- El método HasChanged actualiza cada minuto el estado de la claidad de la red
- La propiedad Client contienen una variable del tipo WiFiClient que se requiere como parámetro para MQTT
- Las propiedades Ssid, Rssi e Ip contienen los valores que hay que completar en los mensajes de datos

*/

#include <ESP8266WiFi.h>


class IIWifi {
public:
	// Métodos
	void Setup();			// Conecta a la Wifi
	bool HasChanged();   	// Cada minuto, actualiza y devuelve true si ha cambiado el valor de Rssi

	// Propiedades
	WiFiClient Client;	    // puntero a la variable global WiFiClient que necesita Mqtt
	char Ssid[40]; 			// parámetros de la wifi para usar en los mensajes de estado
	unsigned int Rssi;
	char Ip[40];
	
private:
	const char* WIFI_SSID       = "infind";
  	const char* WIFI_PASS       = "1518wifi";
};


#endif