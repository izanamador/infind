#include "DHTesp.h" 
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif

DHTesp dht;



WiFiClient wClient;
PubSubClient mqtt_client(wClient);


const char* ssid = "infind";
const char* password = "1518wifi";
const char* mqtt_server = "iot.ac.uma.es";
const char* mqtt_user = "infind";
const char* mqtt_pass = "zancudo";

// usigned var input la variable del led 

StaticJsonDocument<300> doc;
StaticJsonDocument<300> doc2;

#define TAMANHO_MENSAJE 128
#define send_time 30000
unsigned long ultimo_mensaje=0;
char mensaje[TAMANHO_MENSAJE];
char output[300];


// cadenas para topics e ID
char ID_PLACA[16];
char topic_PUB[256];
char topic_SUB[256];

// GPIOs
int LED1 = 2;  
int LED2 = 16; 
  ADC_MODE(ADC_VCC);


//-----------------------------------------------------
void conecta_wifi() {
  Serial.printf("\nConnecting to %s:\n", ssid);
 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.printf("\nWiFi connected, IP address: %s\n", WiFi.localIP().toString().c_str());
}

//-----------------------------------------------------
void conecta_mqtt() {
  // Loop until we're reconnected
  while (!mqtt_client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqtt_client.connect(ID_PLACA, mqtt_user, mqtt_pass)) {
      Serial.printf(" conectado a broker: %s\n",mqtt_server);
      mqtt_client.subscribe(topic_SUB);
    } else {
      Serial.printf("failed, rc=%d  try again in 5s\n", mqtt_client.state());
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

unsigned int level_externo = 0;
unsigned int level_interno = 255;
//-----------------------------------------------------
void procesa_mensaje(char* topic, byte* payload, unsigned int length) {
  char *mensaje = (char *)malloc(length+1); // reservo memoria para copia del mensaje
  strncpy(mensaje, (char*)payload, length); // copio el mensaje en cadena de caracteres
  mensaje[length]='\0'; // caracter cero marca el final de la cadena
  Serial.printf("Mensaje recibido [%s] %s\n", topic, mensaje);
  
  
  sprintf(topic_PUB, "infind/GRUPO3/led/cmd");
  if(strcmp(topic, topic_SUB)==0) 
  {
    
    deserializeJson(doc2,mensaje);
    
    // Serial.println(doc2[0]);
    level_externo = doc2["level"];
    //Serial.printf("Level %d \n", level_externo);
    
    if (level_externo >= 0 && level_externo <= 100) {
      level_interno = 255 - level_externo * 255/100;
      analogWrite(LED2, level_interno);   // Turn the LED on (Note that LOW is the voltage level
      doc2.clear();
      doc2["led"] = level_externo;
      serializeJson(doc2,output);
      snprintf(mensaje, 300, output);
      sprintf(topic_PUB, "infind/GRUPO3/led/status");
      mqtt_client.publish(topic_PUB, mensaje);
      }
  }
  doc2.clear();
  free(mensaje);
}

//-----------------------------------------------------
//     SETUP
//-----------------------------------------------------
void setup() {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);    // inicializa GPIO como salida
  pinMode(LED2, OUTPUT);    
  digitalWrite(LED1, HIGH); // apaga el led
  digitalWrite(LED2, HIGH); 
  analogWrite(LED2, level_interno);
  
  // crea topics usando id de la placa
  sprintf(ID_PLACA, "ESP_%d", ESP.getChipId());
  // sprintf(topic_PUB, "infind/%s/IRENE/temperatura", ID_PLACA);
  // sprintf(topic_SUB, "infind/%s/sub", ID_PLACA);
  sprintf(topic_SUB, "infind/GRUPO3/led/cmd");
  // sprintf(topic_PUB, "infind/GRUPO3/conexion");
  // sprintf(topic_PUB, "infind/GRUPO3/datos");
  // sprintf(topic_PUB, "infind/GRUPO3/led/status");
  
  conecta_wifi();
  mqtt_client.setServer(mqtt_server, 1883);
  mqtt_client.setBufferSize(512); // para poder enviar mensajes de hasta X bytes
  mqtt_client.setCallback(procesa_mensaje);
  conecta_mqtt();
  
  Serial.printf("Identificador placa: %s\n", ID_PLACA );
  Serial.printf("Topic publicacion  : %s\n", topic_PUB);
  Serial.printf("Topic subscripcion : %s\n", topic_SUB);
  Serial.printf("Termina setup en %lu ms\n\n",millis());

  //Usamos el GPIO2
  dht.setup(2, DHTesp::DHT11); // Connect DHT sensor to GPIO 02
}

//-----------------------------------------------------

//-----------------------------------------------------
//     LOOP
//-----------------------------------------------------

void loop() {
  if (!mqtt_client.connected()) conecta_mqtt();
  mqtt_client.loop(); // esta llamada para que la librería recupere el control
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
    // IP
    // RSSI
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
      
      snprintf(mensaje, 300, output);
      Serial.println(output);
      sprintf(topic_PUB, "infind/GRUPO3/datos");
      mqtt_client.publish(topic_PUB, mensaje);
    
    //ES NECESARIO Y PRIORITARIO LIMPIAR EL BUFFER
    doc.clear();
  }
  
  /*
  if (ahora - ultimo_mensaje >= 10000) {
    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();
    
    ultimo_mensaje = ahora;
    snprintf(mensaje, 128, "{\"temperatura\": %f, \"humedad\": %f}", temperature, humidity);
    Serial.println(mensaje);
    mqtt_client.publish(topic_PUB, mensaje);
    digitalWrite(LED2, LOW); // enciende el led al enviar mensaje
  }
  */
  /*if (digitalRead(LED2)==LOW && ahora-ultimo_mensaje>=100) {
    digitalWrite(LED2, HIGH);
  }*/
 

}
