#include "IIDht11.h"

void IIDht11::Setup(IIMqtt *ptMqtt, int pinSda, unsigned int statPeriod) { 
    ptMqtt_     = ptMqtt;
    pinsda_     = pinSda;
    fltTemp_    = 0;
    fltHum_     = 0;
    if (pinsda_ > 0) {
        dht_.setup(pinsda_, DHTesp::DHT11);
        msLastStat_  = 0;
        statPeriod_ = statPeriod;
    }
}

// getters y setters
int   IIDht11::getSda()          { return pinsda_; }
float IIDht11::getTemperature()  { return fltTemp_; }
float IIDht11::getHumidity()     { return fltHum_; }                

      // reporta periódicamente la temperatura y la humedad a menos que el pin sea negativo    
bool IIDht11::HasChanged(void) {         
	if (pinsda_ < 0) { // no hay montado sensor en la placa
		return false; 
	}

	unsigned int now = millis();
  	if (now > msLastStat_ + 1000*statPeriod_) { // leer los sensores y esperar al próximo periodo
    	fltTemp_ = (float)dht_.getTemperature(); 
    	fltHum_  = (float)dht_.getHumidity(); 
        ptMqtt_->RefreshDht11(pinsda_, fltTemp_, fltHum_);
    	Serial.printf("Sensors at %d ms, last=%d \n", now, msLastStat_);
    	msLastStat_ = now;
    	return true; // nueva lectura del sensor disponible
    }

  	return false; // no ha cambiado nada
}    


