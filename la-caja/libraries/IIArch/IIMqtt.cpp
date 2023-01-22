#include "IIMqtt.h"

void IIMqtt::Setup(
		IIWifi *ptWifi,
		void callback (char* strTopicMsg, byte* payload, unsigned int length))
{
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
			return true;
        }
        else {
            Serial.printf("#%d MQTT connection failure. Error rc=%d. Retrying in 5 seconds\n", ++reintentos, MClient.state());
            delay(5000); // espera bloqueante durante 5 segundos antes de reintentar
        }
    } 
	MClient.loop(); 
	return false;
}

