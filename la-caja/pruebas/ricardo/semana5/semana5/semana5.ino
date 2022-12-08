//#include "Arduino.h"

// Objeto - infraestructura
#include "infra.h"
//#include "infra.cpp"
Infra objInfra;


// Objeto - sensor de temperatura
#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif
#include "DHTesp.h" 
#define PIN_DHT11 2 // SENSOR CONECTADO A D2=GPIO4-SDA
#define LED1 2
#define LED2 16
DHTesp objDht;



void mqttCallback(char* topic, byte* payload, unsigned int length) {
  char *mensaje = (char *)malloc(length+1); // reservo memoria para copia del mensaje
  strncpy(mensaje, (char*)payload, length); // copio el mensaje en cadena de caracteres
  mensaje[length]='\0'; // caracter cero marca el final de la cadena
  Serial.printf("Mensaje recibido [%s] %s\n", topic, mensaje);
  // compruebo el topic
  if (strcmp(topic, objInfra.mqttTopicsSub[0])==0) 
  {
    if (mensaje[0] == '1') {
        Serial.printf("Uno\n", topic, mensaje);
        digitalWrite(LED2, LOW);   // Turn the LED on (Note that LOW is the voltage level 
      } else {
        Serial.printf("Cero\n", topic, mensaje);
        digitalWrite(LED2, HIGH);  // Turn the LED off by making the voltage HIGH
      }
  }
  free(mensaje);
}

void setup() {
  // inicializaciones genéricas de la infraestructura
    sprintf(objInfra.mqttTopicPub, "infind/%s/ricardo/dht11", objInfra.espId);
    sprintf(objInfra.mqttTopicsSub[0], "infind/%s/ricardo/sub", objInfra.espId);
    objInfra.Setup(mqttCallback);

  // configuración de pines
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PIN_DHT11, INPUT);
    pinMode(LED1, OUTPUT);    // inicializa GPIO como salida
    pinMode(LED2, OUTPUT);    
    digitalWrite(LED1, HIGH); // apaga el led
    digitalWrite(LED2, HIGH); 

  // inicializaciones específicas
    Serial.println();
    Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)\tHeatIndex (C)\t(F)");
    String thisBoard= ARDUINO_BOARD;
    Serial.println(thisBoard);
    objDht.setup(PIN_DHT11, DHTesp::DHT11); // DHT22->DHT11
}


void loop() {
  objInfra.Loop();

  const unsigned long msInterval = 5000; // five seconds
  const unsigned long msSampling = objDht.getMinimumSamplingPeriod();
  static unsigned long msPrevious = 0; // last update time  
  static long ledState = LOW;          // led lighted
  unsigned long msCurrent = millis(); // current time

  if (msCurrent > msPrevious + msInterval)
  {
    msPrevious = msCurrent;
    Serial.println(ledState==LOW ? "apagando": "encendiendo");  // next action   
    ledState = ledState==LOW ? HIGH: LOW; // set next state
    digitalWrite(LED_BUILTIN, ledState);  // write to led pin
    if (ledState==LOW)
    {
      char strMessage[128];
      delay(msSampling);
      float humidity = objDht.getHumidity();
      float temperature = objDht.getTemperature();
      sprintf(strMessage, "Status=%s, Hum=%0.2f, Temp=%0.1f ºC=%0.1f ºF, Heat=", 
        objDht.getStatusString(), humidity, temperature, objDht.toFahrenheit(temperature));
      Serial.print(strMessage);
      Serial.print(objDht.computeHeatIndex(temperature, humidity, false), 1);
      Serial.println("ºC");
      snprintf(strMessage, MQTT_MAX_MESSAGE, "{\"temperatura\": %0.2f, \"humedad\": %0.2f}", temperature, humidity);
      objInfra.MqttPublish(strMessage);
      //Serial.println(dht.computeHeatIndex(dht.toFahrenheit(temperature), humidity, true), 1);//char strMessage[80];
    }
  }
}


