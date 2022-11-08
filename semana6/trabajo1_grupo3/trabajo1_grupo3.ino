#include "DHTesp.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif

/* Function: CONECTA_WIFI */
#define SSID_ "MiFibra-278E"
#define PASSWORD_ "e7oVWkYw"

/* Function: CONECTA_MQTT */
#define MQTT_SERVER_ "iot.ac.uma.es"
#define MQTT_USER_ "infind"
#define MQTT_PASS_ "zancudo"
#define MQTT_PORT_ 1883

/* TOPICS PUB */
#define TOPIC_DATOS_ "infind/GRUPO3/datos"
#define TOPIC_STATUS_ "infind/GRUPO3/led/status"
#define TOPIC_CONEXION_ "infind/GRUPO3/conexion"

/* TOPICS SUB */
#define TOPIC_CMD_ "infind/GRUPO3/led/cmd"

/* Last Will and Testament */
#define LWT_MESSAGE_ "{\"online\":false}"


/* MQTT CONFIGURATION */
#define MQTT_BUFFER_SIZE_ 512
#define TAMANHO_MENSAJE 128
#define send_time 30000

StaticJsonDocument<300> doc;
StaticJsonDocument<300> doc2;

unsigned long ultimo_mensaje=0;
char mensaje[TAMANHO_MENSAJE];
char output[300];
unsigned int level_externo = 0;
unsigned int level_interno = 255;
char ID_PLACA[16];
char topic_PUB[256];

int LED1 = 2;
int LED2 = 16;

#define totalTopicSubs 1
const char *topicSubs[totalTopicSubs] = { TOPIC_CMD_};


DHTesp dht;
WiFiClient wClient;
PubSubClient mqtt_client(wClient);

/**************************************************************************/
/* El ESP8266 solo cuenta con un ADC (Analogic Digital Converter),        */
/* tenemos que seleccionar el modo de operación en este caso se pedía     */
/* el voltaje de alimentación.                                            */
/**************************************************************************/
/* https://esp8266-arduino-spanish.readthedocs.io/es/latest/reference.html#entrada-analogica */
ADC_MODE(ADC_VCC);


/**************************************************************************/
/* conecta_wifi()                                                         */
/*    Realiza la conexión del módulo a la WIFI definida en las macros     */
/**************************************************************************/
void conecta_wifi() {
  Serial.printf("\nConnecting to %s:\n", SSID_);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID_, PASSWORD_);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }

  Serial.printf("\nWiFi connected, IP address: %s\n", WiFi.localIP().toString().c_str());
}


/**************************************************************************/
/* conecta_mqtt()                                                         */
/*    Realiza la conexión con el broker MQTT y se suscribe a los topics   */
/*    especificados en la variable topicssubs                             */
/**************************************************************************/
void conecta_mqtt() {
  int i;

  // Loop until we're reconnected
  while (!mqtt_client.connected()) {

    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if (mqtt_client.connect(ID_PLACA, MQTT_USER_, MQTT_PASS_,TOPIC_CONEXION_,1,true,LWT_MESSAGE_)) {

      Serial.printf("Conectado a broker: %s\n",MQTT_SERVER_);
      for(i = 0; i < totalTopicSubs; ++i){
        mqtt_client.subscribe(topicSubs[i]);
      }
    } else {

      Serial.printf("failed, rc=%d  try again in 5s\n", mqtt_client.state());

      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

/**************************************************************************/
/* procesa_mensaje()                                                      */
/*    Procesa los callbacks recibidos desde el servidor MQTT, para ello   */
/*    debe estar suscrito al topic en la función conecta_mqtt()           */
/**************************************************************************/
void procesa_mensaje(char* topic, byte* payload, unsigned int length) {

  /* reservo memoria para copiar del mensjae */
  char *mensaje = (char *)malloc(length+1);

  /* copio el mensaje en cadena de caracteres */
  strncpy(mensaje, (char*)payload, length);

  mensaje[length]='\0'; // caracter cero marca el final de la cadena
  //  Serial.printf("Mensaje recibido [%s] %s\n", topic, mensaje);

  if(strcmp(topic, TOPIC_CMD_)==0){

      deserializeJson(doc2,mensaje);

      /* Serial.println(doc2[0]); */
      level_externo = doc2["level"];

      /* Serial.printf("Level %d \n", level_externo); */
      if (level_externo >= 0 && level_externo <= 100) {
        level_interno = 255 - level_externo * 255/100;

        /* Turn the LED on (Note that LOW is the voltage level */
        analogWrite(LED2, level_interno);

        doc2.clear();
        doc2["led"] = level_externo;
        serializeJson(doc2,output);
        snprintf(mensaje, 300, output);
        sprintf(topic_PUB, TOPIC_STATUS_);
        mqtt_client.publish(topic_PUB, mensaje);
      }
    }
  doc2.clear();
  free(mensaje);
}


/**************************************************************************/
/* SETUP                                                                  */
/**************************************************************************/
void setup() {
  int i;
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);    // inicializa GPIO como salida
  pinMode(LED2, OUTPUT);

  analogWrite(LED1, level_interno);
  analogWrite(LED2, level_interno);

  /* crea topics usando id de la placa */
  sprintf(ID_PLACA, "ESP_%d", ESP.getChipId());

  conecta_wifi();
  mqtt_client.setServer(MQTT_SERVER_, MQTT_PORT_);

  /* para poder enviar mensajes de hasta X bytes */
  mqtt_client.setBufferSize(MQTT_BUFFER_SIZE_);
  mqtt_client.setCallback(procesa_mensaje);
  conecta_mqtt();

  Serial.printf("Identificador placa: %s\n", ID_PLACA );
  Serial.printf("Topic publicacion  : %s, %s, %s \n", TOPIC_CONEXION_,TOPIC_DATOS_,TOPIC_STATUS_);

  Serial.printf("Topic subscripcion : ");
  for(i=0;i<totalTopicSubs;i++){
    Serial.printf("%s ",topicSubs[i]);
  }

  Serial.printf("\nTermina setup en %lu ms\n\n",millis());

  /* Conectamos el sensor al GPIO2 */
  dht.setup(2, DHTesp::DHT11);
  mqtt_client.publish(TOPIC_CONEXION_,"{\"online\":true}",true);
}


/**************************************************************************/
/* LOOP                                                                   */
/**************************************************************************/
void loop() {

  if (!mqtt_client.connected()) conecta_mqtt();

  /* LLamada para que la libreria MQTT recupere el control */
  mqtt_client.loop();

  unsigned long ahora = millis();

  if (ahora - ultimo_mensaje >= send_time) {

    // Sensor de temperatura
    float temperature = dht.getTemperature();

    // Sensor de humedad
    float humidity = dht.getHumidity();

    // bateria/alimentacion
    int bateria = ESP.getVcc();

    // SSID
    String name_SSID = WiFi.SSID();

    doc["Uptime"]= ahora;
    doc["Vcc"] = bateria/1000;

    JsonObject DHT11_sensor = doc.createNestedObject("DHT11");
    DHT11_sensor["temp"] = temperature;
    DHT11_sensor["hum"] = humidity;
    doc["LED"] = level_externo;

    JsonObject Wifi = doc.createNestedObject("Wifi");
    Wifi["SSId"] = "infind";
    Wifi["IP"] = WiFi.localIP().toString();
    Wifi["RSSI"] = WiFi.RSSI();

    serializeJson(doc,output);
    ultimo_mensaje = ahora;

    mqtt_client.publish(TOPIC_DATOS_, output);

    //ES NECESARIO Y PRIORITARIO LIMPIAR EL BUFFER
    doc.clear();
  }
}
