#include "IIMqtt.h"

void IIMqtt::Setup(
		IIWifi *ptWifi,
		void callback (char* strTopicMsg, byte* payload, unsigned int length))
{
	ptWifi_ = ptWifi;
	MClient.setClient(ptWifi->WClient);
    MClient.setServer(MQTT_SERVER, 1883);
    MClient.setBufferSize(MQTT_MAX_BUFFER); 
    MClient.setCallback(callback);
    sprintf(espid_, "ESP_%d", ESP.getChipId());
	sprintf(TopicConnE, "II3/conn/esp/%s", espid_);
	sprintf(TopicDataE, "II3/conn/esp/%s", espid_);
	sprintf(TopicConfA, "II3/conf/all");
	sprintf(TopicConfE, "II3/conf/esp/%s", espid_);
	sprintf(TopicLedcA, "II3/ledc/all");
	sprintf(TopicLedcE, "II3/ledc/esp/%s", espid_);
	sprintf(TopicLedsE, "II3/leds/esp/%s", espid_);
	sprintf(TopicFotcA, "II3/fotc/all");
	sprintf(TopicFotcE, "II3/fotc/esp/%s", espid_);
	sprintf(TopicGamcA, "II3/gamc/all");
	sprintf(TopicGamc1, "II3/gamc/1/chrono/%s", espid_);
	sprintf(TopicGamc2, "II3/gamc/2/numpad/%s", espid_);
	sprintf(TopicGamc3, "II3/gamc/3/simon/%s", espid_);
	sprintf(TopicGamc4, "II3/gamc/4/joystick/%s", espid_);
	sprintf(TopicGamc5, "II3/gamc/5/switch/%s", espid_);
	sprintf(TopicGamc6, "II3/gamc/6/morse/%s", espid_);
	sprintf(TopicGams1, "II3/gams/1/chrono/%s", espid_);
	sprintf(TopicGams2, "II3/gams/2/numpad/%s", espid_);
	sprintf(TopicGams3, "II3/gams/3/simon/%s", espid_);
	sprintf(TopicGams4, "II3/gams/4/joystick/%s", espid_);
	sprintf(TopicGams5, "II3/gams/5/switch/%s", espid_);
	sprintf(TopicGams6, "II3/gams/6/morse/%s", espid_);
}

void IIMqtt::Subscribe(char* strTopic) {
	Serial.printf("Suscribing to topic: [%s]\n", strTopic);
	MClient.subscribe(strTopic);
}


bool IIMqtt::Message(char* strTopic, byte* payload, unsigned int length)
{
	#define JSON_MESSAGE_SIZE    1024
	StaticJsonDocument<JSON_MESSAGE_SIZE> jsonSub;

  //----- Mensaje recibido
    char *strMsg = (char *)malloc(length+1);
    strncpy(strMsg, (char*)payload, length);
    strMsg[length]='\0';
    Serial.printf("\n\n\nRecibido por Topic:%s\n%s\n----------------\n", strTopic, strMsg);
    deserializeJson(jsonSub,strMsg);
    free(strMsg);
    char output[1000];
    serializeJsonPretty(jsonSub, output);
    Serial.println(output);

  //----- Procesamiento de mensajes de juego
    if ((strcmp(strTopic, TopicConfA)==0)||(strcmp(strTopic, TopicConfE)==0)) {
    	dataeperiod = jsonSub["dataeperiod"];
		dht11Tempmin_ = jsonSub["tempmin"];
		dht11Tempmax_ = jsonSub["tempmax"];
		dht11Hummin_ = jsonSub["hummin"];
		dht11Hummax_ = jsonSub["hummax"];
    	Serial.printf("New reporting period set to %d seconds", dataeperiod);
    }
    return true;
}



bool IIMqtt::HasChanged() {
	static int reintentos = 0;


	while (!MClient.connected()){
		//--- preparar el mensaje de lastwill para cuando quede offline
		char msgOnline[50];
		sprintf(msgOnline, "{\"CHIPID\":\"%s\",\"online\":false}", espid_);
	
      	Serial.printf("\n\n%d try to connect to [%s] MQTT server\n", ++reintentos, MQTT_SERVER);
      	if (MClient.connect(espid_, MQTT_USER, MQTT_PASS, TopicConnE, 1, true, msgOnline)) {
            Serial.printf("Successful connection to [%s]\n", MQTT_SERVER);
            Subscribe(TopicConfA);
			Subscribe(TopicConfE); 
			Subscribe(TopicLedcA); 
 			Subscribe(TopicLedcE); 
			Subscribe(TopicFotcA); 
			Subscribe(TopicFotcE); 
			Subscribe(TopicGamcA); 
			Subscribe(TopicGamc1); 
			Subscribe(TopicGamc2); 
			Subscribe(TopicGamc3); 
			Subscribe(TopicGamc4); 
			Subscribe(TopicGamc5); 
			Subscribe(TopicGamc6); 

			//--- preparar y enviar el mensaje de online
			sprintf(msgOnline, "{\"CHIPID\":\"%s\",\"online\":true}", espid_);
			MClient.publish(TopicConnE, msgOnline, true);

			//--- publicar periódicamente el estado de los sensores
			PublishDataE();
			return true;
        }
        else {
            Serial.printf("#%d MQTT connection failure. Error rc=%d. Retrying in 5 seconds\n", ++reintentos, MClient.state());
            delay(5000); // espera bloqueante durante 5 segundos antes de reintentar
        }
    } 
	MClient.loop(); 
	PublishDataE();
	return false;
}

void IIMqtt::RefreshDht11(int pinSda, float fltTemp, float fltHum){
	static int iAlarmTemp = 0;
	static int iAlarmHum = 0;
    dht11Pinsda_ = pinSda;
    dht11Temp_   = fltTemp;
    dht11Hum_  = fltHum;
    if (dht11Temp_ < dht11Tempmin_){
    	sprintf(dht11AlarmTemp_, "Alarm-Temp %0.2f < %02.f too low", dht11Temp_, dht11Tempmin_);
    } else if (dht11Temp_ > dht11Tempmax_) {
    	sprintf(dht11AlarmTemp_, "Alarm-Temp %0.2f > %02.f too high", dht11Temp_, dht11Tempmin_);
    } else {
    	sprintf(dht11AlarmTemp_, "Temp ok");
    }
    if (dht11Hum_ < dht11Hummin_){
    	sprintf(dht11AlarmHum_, "Alarm-Hum %0.2f < %02.f too low", dht11Hum_, dht11Hummin_);
    } else if (dht11Hum_ > dht11Hummax_) {
    	sprintf(dht11AlarmHum_, "Alarm-Hum %0.2f > %02.f too high", dht11Hum_, dht11Hummin_);
    } else {
    	sprintf(dht11AlarmHum_, "Hum ok");
    }
}

void IIMqtt::PublishDataE(bool immediate){
	static unsigned int msLastSent = 0;
	unsigned int now = millis();
	if (immediate || (now > msLastSent + 1000*dataeperiod)) {
		Serial.printf("Enviando \n");
	  	// componemos el mensaje
	        StaticJsonDocument<500> doc;
	        doc["CHIPID"]               = espid_;
	        doc["online"]               = true;
	        doc["esp"]["uptime"]        = millis(); // "2509853";
	        doc["esp"]["vcv"]           = ESP.getVcc()/1000; // batería de alimentación en voltios
	        doc["wifi"]["ssid"]         = ptWifi_->Ssid; 
	        doc["wifi"]["rssi"]         = ptWifi_->Rssi;
	        doc["wifi"]["ip"]           = ptWifi_->Ip;
	        doc["dht11"]["pinsda"]      = dht11Pinsda_;
	        doc["dht11"]["temp"]        = dht11Temp_;	        
	        doc["dht11"]["hum"]         = dht11Hum_;
			doc["dht11"]["alarmtemp"]   = dht11AlarmTemp_;
			doc["dht11"]["alarmhum"]    = dht11AlarmHum_;
			

      	// imprimimos el mensaje formateado para que sea más fácil de ver
	        char strSerialized[1000];
	        serializeJsonPretty(doc, strSerialized);
	        Serial.printf("Enviando por %s\n---------\n%s\n---------\n", TopicDataE, strSerialized);
	        Serial.println();

      	// serializamos sin formato para que ocupe menos al enviarlo por la wifi
        	serializeJson(doc,strSerialized);
        	MClient.publish(TopicDataE, strSerialized);
        	doc.clear();
			msLastSent = now;
	}
}


