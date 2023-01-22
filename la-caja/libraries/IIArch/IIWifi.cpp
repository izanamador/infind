#include "IIWifi.h"


void IIWifi::Setup() {
	static int reintentos = 0;

    // Comment("conecta_wifi", 1, "inicialización a partir de los parámetros de conexión");
      Serial.printf("\nConnecting to Wifi [%s]:\n", WIFI_SSID); 
      WiFi.mode(WIFI_STA);                    // WiFi y WIFI_STA vienen de la librería
      WiFi.begin(WIFI_SSID, WIFI_PASS);  

  //--- reintento de conexión cada 0.2 segundos
    while (WiFi.status() != WL_CONNECTED) {   // WL_CONNECTED también viene de la librería
      delay(200);                             // espera bloqueante!!!
      Serial.print(".");                      // feedback al usuario
    
      if (reintentos++ % 100 == 0) {        
        Serial.printf("\nRetry number %02d : ", reintentos);
      }
    }

  //--- actualizamos propiedades
 	strcpy(Ssid, WiFi.SSID().c_str());
 	Rssi = WiFi.RSSI();
 	strcpy(Ip, WiFi.localIP().toString().c_str());
       
  //--- conexión WiFi disponible para MQTT
    Serial.printf("\nSuccessful connection to Wifi [%s]\n RSSI: %d\n IP Address: %s\n", Ssid, Rssi, Ip);
}

bool IIWifi::HasChanged() {
	static unsigned int msLaststat = millis();
	unsigned int msNow = millis();

	if (msNow > msLaststat + 60*1000) {		
		msLaststat = msNow;
		int newRssi = WiFi.RSSI();
		if (newRssi != Rssi) {
			Rssi = newRssi;
			return true;
		}
	}
	return false;
}