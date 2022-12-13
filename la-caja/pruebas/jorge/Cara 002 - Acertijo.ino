#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <string.h>
//#include <math.h>
#include <iostream>
//using namespace std;

#define TOPIC_CONEXION_ "II3/ESP1/conexion"

WiFiClient wClient;
PubSubClient mqtt_client(wClient);

const char* ssid = "infind";
const char* password = "1518wifi";
const char* mqtt_server = "iot.ac.uma.es";
const char* mqtt_user = "II3";
const char* mqtt_pass = "qW30SImD";

// cadenas para topics e ID
char ID_PLACA[16];
char topic_PUB[256];
char topic_SUB[256];

// GPIOs
int LED1 = 2;
int LED2 = 16;
int ans = -1;
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
      mqtt_client.subscribe(TOPIC_CONEXION_);
    } else {
      Serial.printf("failed, rc=%d  try again in 5s\n", mqtt_client.state());
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//-----------------------------------------------------
void procesa_mensaje(char* topic, byte* payload, unsigned int length) {
  char *mensaje = (char *)malloc(length+1); // reservo memoria para copia del mensaje
  strncpy(mensaje, (char*)payload, length); // copio el mensaje en cadena de caracteres
  mensaje[length]='\0'; // caracter cero marca el final de la cadena
  Serial.printf("Mensaje recibido [%s] %s\n", topic, mensaje);
  // compruebo el topic

  //ans = 77;
   if(strcmp(topic, TOPIC_CONEXION_)==0){
     ans = atoi(mensaje);
  }

  Serial.println(ans);

  free(mensaje);
}

//-----------------------------------------------------
//     SETUP
//-----------------------------------------------------
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Empieza setup...");
  pinMode(LED1, OUTPUT);    // inicializa GPIO como salida
  pinMode(LED2, OUTPUT);
  digitalWrite(LED1, LOW); // apaga el led
  digitalWrite(LED2, HIGH);
  // crea topics usando id de la placa
  strcpy(ID_PLACA,"ESP1");
  sprintf(topic_SUB, "II3/ESP1/conexion");
  conecta_wifi();
  
  mqtt_client.setServer(mqtt_server, 1883);
  mqtt_client.setBufferSize(512); // para poder enviar mensajes de hasta X bytes
  mqtt_client.setCallback(procesa_mensaje);
  conecta_mqtt();

  Serial.printf("Identificador placa: %s\n", ID_PLACA );
  Serial.printf("Topic publicacion  : %s\n", topic_PUB);
  Serial.printf("Topic subscripcion : %s\n", topic_SUB);
  Serial.printf("Termina setup en %lu ms\n\n",millis());

}

//-----------------------------------------------------
#define TAMANHO_MENSAJE 128
unsigned long ultimo_mensaje=0;
unsigned long ultimo_mensaje_dht=0;
char mensaje[TAMANHO_MENSAJE];
//-----------------------------------------------------
//     LOOP
//-----------------------------------------------------

int GetData(){
  
  static int ans_get;
//while (Serial.available() == 0) {}     //wait for data available
   if (Serial.available() > 0) {
  String teststr = Serial.readString();  //read until timeout
  teststr.trim();                        // remove any \r \n whitespace at the end of the String
  ans_get = teststr.toInt();
  }
  return ans_get;
}



void loop() {
  static int ans_user = 0;
   if (!mqtt_client.connected()) conecta_mqtt();
  mqtt_client.loop(); // esta llamada para que la librería recupere el control
  
  ans_user = GetData();
    if (ans_user == ans){
     Serial.printf("You win!");
  }
  Serial.println(ans);
  Serial.println(ans_user);
}
