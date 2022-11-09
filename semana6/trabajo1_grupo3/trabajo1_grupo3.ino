#include "DHTesp.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

/* Function: CONECTA_WIFI */
#define SSID_ "MiFibra-278E"
#define PASSWORD_ "e7oVWkYw"
#define WIFI_RETRY_DELAY_ 200

/* Function: CONECTA_MQTT */
#define MQTT_SERVER_ "iot.ac.uma.es"
#define MQTT_USER_ "infind"
#define MQTT_PASS_ "zancudo"
#define MQTT_PORT_ 1883
#define MQTT_RETRY_DELAY_ 5000

/* Function: PROCESA_MENSAJE */
#define MAX_LED_VALUE_ 100
#define MIN_LED_VALUE_ 0
#define MAX_SAMPLE_VALUE_ 255 //2^(8)-1

/* TOPICS PUB */
#define TOPIC_DATOS_ "infind/GRUPO3/datos"
#define TOPIC_STATUS_ "infind/GRUPO3/led/status"
#define TOPIC_CONEXION_ "infind/GRUPO3/conexion"

/* TOPICS SUB */
#define TOPIC_CMD_ "infind/GRUPO3/led/cmd"
#define TOTAL_TOPICS_SUBS_ 1

/* LAST WILL AND TESTAMENT */
#define LWT_MESSAGE_ "{\"online\":false}"

/* HARDWARE PIN NAME */
#define LED1_ 2
#define LED2_ 16

/* SETUP */
#define MQTT_BUFFER_SIZE_ 512
#define INIT_MESSAGE_CONNECTION_ "{\"online\":true}"
#define DHT_GPIO_ 2

/* LOOP */
#define SEND_TIME_ 30000

/* GLOBAL */
#define MESSAGE_SIZE_ 300

char ID_PLACA[16];

StaticJsonDocument<MESSAGE_SIZE_> json_enviado;
StaticJsonDocument<MESSAGE_SIZE_> json_recibido;

unsigned int level_externo = MIN_LED_VALUE_;
unsigned int level_interno = MAX_SAMPLE_VALUE_;
const char *topicSubs[TOTAL_TOPICS_SUBS_] = { TOPIC_CMD_};

DHTesp dht;
WiFiClient wClient;
PubSubClient mqtt_client(wClient);

/**************************************************************************/
/* El ESP8266 solo cuenta con un ADC (Analogic Digital Converter),        */
/* tenemos que seleccionar el modo de operación, en este caso se pedía    */
/* el voltaje de alimentación.                                            */
/**************************************************************************/
/* https://esp8266-arduino-spanish.readthejson_enviados.io/es/latest/reference.html#entrada-analogica */
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
    delay(WIFI_RETRY_DELAY_);
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

  // Loop until we're reconnected
  while (!mqtt_client.connected()) {

    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if (mqtt_client.connect(ID_PLACA, MQTT_USER_, MQTT_PASS_,TOPIC_CONEXION_,1,true,LWT_MESSAGE_)) {

      Serial.printf("Conectado a broker: %s\n",MQTT_SERVER_);
      for(int i = 0; i < TOTAL_TOPICS_SUBS_; ++i){
        mqtt_client.subscribe(topicSubs[i]);
      }
    } else {

      Serial.printf("failed, rc=%d  try again in 5s\n", mqtt_client.state());

      // Wait 5 seconds before retrying
      delay(MQTT_RETRY_DELAY_);
    }
  }
}

/**************************************************************************/
/* procesa_mensaje()                                                      */
/*    Procesa los callbacks recibidos desde el servidor MQTT, para ello   */
/*    debe estar suscrito al topic en la función conecta_mqtt()           */
/**************************************************************************/
void procesa_mensaje(char* topic, byte* payload, unsigned int length) {

  static char message_send[MESSAGE_SIZE_];
  /* reservo memoria para copiar del mensjae */
  char *mensaje_recibido = (char *)malloc(length+1);
  /* copio el mensaje_recibido en cadena de caracteres */
  strncpy(mensaje_recibido, (char*)payload, length);

  mensaje_recibido[length]='\0'; // caracter cero marca el final de la cadena

  if(strcmp(topic, TOPIC_CMD_)==0){

      deserializeJson(json_recibido,mensaje_recibido);

      level_externo = json_recibido["level"];

      if (level_externo >= MIN_LED_VALUE_ && level_externo <= MAX_LED_VALUE_) {
        level_interno = MAX_SAMPLE_VALUE_ - level_externo * MAX_SAMPLE_VALUE_/MAX_LED_VALUE_;

        analogWrite(LED2_, level_interno);

        /* HAY QUE LIBERAR/LIMPIAR EL BUFFER */
        json_recibido.clear();
        json_recibido["led"] = level_externo;

        serializeJson(json_recibido,message_send);

        mqtt_client.publish(TOPIC_STATUS_, message_send);
      }
    }
  /* HAY QUE LIBERAR/LIMPIAR EL BUFFER */
  json_recibido.clear();
  free(mensaje_recibido);
}

/**************************************************************************/
/* SETUP                                                                  */
/**************************************************************************/
void setup() {
  Serial.begin(115200);
  /* inicializa GPIO como salida */
  pinMode(LED1_, OUTPUT);
  pinMode(LED2_, OUTPUT);

  analogWrite(LED1_, level_interno);
  analogWrite(LED2_, level_interno);

  sprintf(ID_PLACA, "ESP_%d", ESP.getChipId());

  conecta_wifi();
  mqtt_client.setServer(MQTT_SERVER_, MQTT_PORT_);
  /* para poder enviar mensajes de hasta X bytes */
  mqtt_client.setBufferSize(MQTT_BUFFER_SIZE_);
  /* Selecciona la función que procesa una llamada del broker */
  mqtt_client.setCallback(procesa_mensaje);
  conecta_mqtt();

  Serial.printf("Identificador placa: %s\n", ID_PLACA );
  Serial.printf("Topic publicacion  : %s, %s, %s \n", TOPIC_CONEXION_,TOPIC_DATOS_,TOPIC_STATUS_);

  Serial.printf("Topic subscripcion : ");
  for(int i=0;i<TOTAL_TOPICS_SUBS_;i++){
    Serial.printf("%s ",topicSubs[i]);
  }

  Serial.printf("\nTermina setup en %lu ms\n\n",millis());

  /* Conectamos el sensor al GPIO2 */
  dht.setup(DHT_GPIO_, DHTesp::DHT11);
  mqtt_client.publish(TOPIC_CONEXION_,INIT_MESSAGE_CONNECTION_,true);
}

/**************************************************************************/
/* LOOP                                                                   */
/**************************************************************************/
void loop() {
  static unsigned long ultimo_mensaje = 0;
  static char message_send[MESSAGE_SIZE_];

  if (!mqtt_client.connected()) conecta_mqtt();

  /* LLamada para que la libreria MQTT recupere el control */
  mqtt_client.loop();

  unsigned long ahora = millis();

  if (ahora - ultimo_mensaje >= SEND_TIME_) {

    json_enviado["Uptime"]= ahora;

    /* bateria/alimentacion en voltios*/
    json_enviado["Vcc"] = ESP.getVcc()/1000;

    JsonObject DHT11_sensor = json_enviado.createNestedObject("DHT11");
    /* Sensor de temperatura */
    DHT11_sensor["temp"] = dht.getTemperature();
    /* Sensor de humedad */
    DHT11_sensor["hum"] = dht.getHumidity();

    json_enviado["LED"] = level_externo;

    JsonObject Wifi = json_enviado.createNestedObject("Wifi");
    /* SSID */
    Wifi["SSId"] = WiFi.SSID();
    /* IP */
    Wifi["IP"] = WiFi.localIP().toString();
    /* RSSI */
    Wifi["RSSI"] = WiFi.RSSI();

    serializeJson(json_enviado,message_send);
    ultimo_mensaje = ahora;

    mqtt_client.publish(TOPIC_DATOS_, message_send);

    /* HAY QUE LIBERAR/LIMPIAR EL BUFFER */
    json_enviado.clear();
  }
}
