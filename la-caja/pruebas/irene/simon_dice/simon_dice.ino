#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Arduino_JSON.h>


//***************************************
// Declaración de variables y constantes
//***************************************

  /* Function: CONECTA_WIFI */
  #define SSID_ "infind"
  #define PASSWORD_ "1518wifi"
  
  /* Function: CONECTA_MQTT */
  #define MQTT_SERVER_ "iot.ac.uma.es"
  #define MQTT_USER_ "II3"  //"infind"
  #define MQTT_PASS_ "qW30SImD"//"zancudo"


  /* TOPICS PUB */
  //#define TOPIC_PUB_ "II3/ESP" + String(ESP.getChipId()) + "/resultados_juego2"
  #define TOPIC_PUB_ "II3/ESP14440037/resultados_juego2"
  #define TOPIC_PUB_ESTADO_ "II3/ESP14440037/estado_juego2"
  
  /* TOPICS SUB */
  //#define TOPIC_SUB_ "II3/ESP" + String(ESP.getChipId()) + "/datos_juego2"
  #define TOPIC_SUB_ "II3/ESP14440037/datos_juego2"

 /* HARDWARE PIN NAME */
 // Pins de salida para los LED
  #define LED_ROJO_ 5           // D1-5      
  #define LED_AZUL_ 4           // D2-4 
  #define LED_AMARILLO_ 2       //  D4-2
  #define LED_VERDE_ 14         //  D5-14
  
 // Pin para el ZUMBADOR_ piezoelectrico
  #define ZUMBADOR_ 12          // D6-12
  #define tonePin 12
  
  // Pins de salida para los botones
  #define BOTON_ROJO_ 13        //  D7-13
  #define BOTON_AZUL_ 16        //  D0-16 // ojo, el 15 no va
  #define BOTON_AMARILLO_ 10    // SSD3-10
  #define BOTON_VERDE_ 0        // D3-0 // ojo, SSD2-9 no lee bien


  /* SETUP */

  
  /* LOOP */
  int state = 0;                    // Variable que indica el estado del juego: 0 esperando, 1 en el juego, 2 fin de juego
  int reducir_secuencia = 0;        // Variable que reduce la puntuación máxima si se pide ayuda
  int HIGH_PWM = 0;                 // Variable que realiza el control PWM de los leds
  long unsigned tiempo_inicio = 0;
  long unsigned tiempo_partida_total = 0;
  long unsigned tiempo_partida_actual = 0;
  int aciertos = 0;                 // Variable que ayuda al control PWM de los leds, se reinicia al perder cada partida
  int aciertos_por_partida = 0;     // Variable que indica los aciertos en cada partida, se reinicia tras cargar su valor en el json a publicar por mqtt
  int actualiza = 0;                // Variable que indica que se ha perdido la partida y se han de actualizar ciertos valores

  
  /* GLOBAL */
  #define MESSAGE_SIZE_ 300
  long sequence[20];             // Array que alberga la secuencia
  int contador = 0;              // Contador
  long input = 5;                // Indicador de boton pulsado
  int wait = 500;                // Retraso segun la secuencia se incrementa
  int puntuacion_maxima = 5;     // Puntuación máxima donde acaba el juego 
  char ID_PLACA[16];
  int una_vez = 0;
  int CLK_=3;
  #define CLOCK_STOP 0
  #define CLOCK_START 1
  #define CLOCK_RESET 2
  #define CLOCK_PAUSE 3
  #define CLOCK_CONTINUE 4

  /* Function: PROCESA_MENSAJE */
  StaticJsonDocument<MESSAGE_SIZE_> json_recibido;
 
  /* Function: ACTUALIZACION_NODERED */
  static char message_send[MESSAGE_SIZE_];
  StaticJsonDocument<MESSAGE_SIZE_> jsonRoot;
   
  /* Function: FELICITACION */
  int length = 15;                                                // Numero de notas de la melodia
  char notes[] = "ccggaagffeeddc ";                               // Notas de la melodia (cada letra es una nota distinta)
  int beats[] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 4 };  // Duracion de cada tono en un array
  int tempo = 100;                                                // Tempo de la melodia
  int cancion_Mario = 0;                                          // Variable que activa la melodía de Mario al ganar el juego


  WiFiClient wClient;
  PubSubClient mqtt_client(wClient);

 
//*******************************************
// Creando mensaje JSON para enviar a NodeRed
//*******************************************
void actualizacion_NodeRed()
{    
    jsonRoot.clear();
    
    jsonRoot["tiempo_partida_total"] = tiempo_partida_total;
    jsonRoot["tiempo_partida_actual"] = tiempo_partida_actual;
    jsonRoot["aciertos_por_partida"] = aciertos_por_partida;
    jsonRoot["ayudas_solicitadas"] = 5 - puntuacion_maxima;
    jsonRoot["state"] = state;

    // Serializacion y envio mqtt
    serializeJson(jsonRoot,message_send);
    mqtt_client.publish(TOPIC_PUB_, message_send);
            
    Serial.print("Message published: ");
    Serial.println(message_send);
    Serial.print("Using topic: ");
    Serial.println(TOPIC_PUB_);
            
    jsonRoot.clear(); // hay que limpiar el buffer
}

void actualizacion_estado()
{    
    jsonRoot.clear();
    
    jsonRoot["state"] = state;
    jsonRoot["clock"] = CLK_;
    
    // Serializacion y envio mqtt
    serializeJson(jsonRoot,message_send);
    mqtt_client.publish(TOPIC_PUB_ESTADO_, message_send);
               
    jsonRoot.clear(); // hay que limpiar el buffer
}
//********************************
// Conectando mqtt
//********************************
void conecta_mqtt() {
  // Loop until we're reconnected
  while (!mqtt_client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqtt_client.connect(ID_PLACA, MQTT_USER_, MQTT_PASS_)) {
      Serial.printf(" conectado a broker: %s\n",MQTT_SERVER_);
      mqtt_client.subscribe(TOPIC_SUB_);
    } else {
      Serial.printf("failed, rc=%d  try again in 5s\n", mqtt_client.state());
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


//********************************
// Conectando wifi
//********************************
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


//********************************
// Recepción mensaje
//********************************
void procesa_mensaje(char* topic, byte* payload, unsigned int length) {

  char *mensaje_recibido = (char *)malloc(length+1);
  strncpy(mensaje_recibido, (char*)payload, length);    // copio el mensaje_recibido en cadena de caracteres
  mensaje_recibido[length]='\0';                        // caracter cero marca el final de la cadena
  Serial.print("Mensaje recibido"); 
  Serial.println(mensaje_recibido);

  if(strcmp(topic, TOPIC_SUB_)==0){

      deserializeJson(json_recibido,mensaje_recibido);  // leo de JSON
      state = json_recibido["state"]; 
      reducir_secuencia = json_recibido["reducir_secuencia"];
      }
     
  json_recibido.clear();                                // limpiar buffer
  free(mensaje_recibido);                               // liberar buffer
}




//********************************
// Setup
//********************************
  void setup() {
   // Configuración de los pines de los leds y del ZUMBADOR_ como salidas
    pinMode(LED_ROJO_, OUTPUT);      
    pinMode(LED_VERDE_, OUTPUT); 
    pinMode(LED_AMARILLO_, OUTPUT); 
    pinMode(LED_AZUL_, OUTPUT); 
    pinMode(ZUMBADOR_, OUTPUT);
      
   // Configuración de los pines de los botones como entradas
    pinMode(BOTON_ROJO_, INPUT);    
    pinMode(BOTON_VERDE_, INPUT);
    pinMode(BOTON_AMARILLO_, INPUT);
    pinMode(BOTON_AZUL_, INPUT);
 
    // Melodia de inicio al arrancar el Arduino    
    felicitacion();

    Serial.begin(115200);
    conecta_wifi();
    mqtt_client.setServer(MQTT_SERVER_, 1883);
    mqtt_client.setBufferSize(512); // para poder enviar mensajes de hasta X bytes
    mqtt_client.setCallback(procesa_mensaje);
    conecta_mqtt();
  }

 
//********************************
// Programa principal
//********************************
  void loop() {
    if (!mqtt_client.connected()) conecta_mqtt();
    mqtt_client.loop(); // esta llamada para que la librería recupere el control
    
    if (una_vez== 0 && state!=0){
    actualizacion_estado();
    if (state== 2){
       una_vez=1;
       CLK_ = CLOCK_STOP;
    }
    }
    if (state == 1)
    {
      CLK_ = CLOCK_START;
      if (reducir_secuencia ==1)
      {
        puntuacion_maxima = puntuacion_maxima - 1;
        reducir_secuencia = 0;
      }
        HIGH_PWM = round((255/puntuacion_maxima) + ((255*aciertos)/puntuacion_maxima)); // Control PWM de los led
        tiempo_inicio = millis();
        //tiempo_partida_actual = tiempo_partida_total;
        mostrar_secuencia();  // Reproduce la sequencia
        leer_secuencia();     // Lee la sequencia
        delay(1000);          // Espera 1 segundo
        
        if (state==2 || actualiza==1)
        {
            tiempo_partida_actual = millis() - tiempo_inicio;
            tiempo_partida_total = tiempo_partida_total + tiempo_partida_actual;
            actualizacion_NodeRed();
            
            actualiza = 0;
            aciertos_por_partida = 0;
            cancion_Mario = 0;
        }
    }
  }
  
  
//********************************
// Declaración de funciones
//******************************** 
  // Funcion para definir las notas y sus frecuencias
  void playNote(char note, int duration) {
    char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
    int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };
   
    for (int i = 0; i < 8; i++) {
       if (names[i] == note) {
         playtone(tones[i], duration);
       }
    }
  }
 
 
 
 // Funcion para definir las notas segun la duración y el tono
  void playtone(int tone, int duration) {
    for (long i = 0; i < duration * 1000L; i += tone *2) {
      digitalWrite(ZUMBADOR_, HIGH);
      delayMicroseconds(tone);
      digitalWrite(ZUMBADOR_, LOW);
      delayMicroseconds(tone);
    }
  }
     
  
 
 // Funciones para encender los leds y reproducir el tono correspondiente
  void flash_rojo() {
    analogWrite(LED_ROJO_, HIGH_PWM);
    playtone(2273,wait);            
    digitalWrite(LED_ROJO_, LOW);
  }
  
  void flash_azul() {
    analogWrite(LED_AZUL_, HIGH_PWM);
    playtone(1700,wait);            
    digitalWrite(LED_AZUL_, LOW);
  }
  
  void flash_amarillo() {
    analogWrite(LED_AMARILLO_, HIGH_PWM);
    playtone(1275,wait);             
    digitalWrite(LED_AMARILLO_, LOW);
  } 
  
  void flash_verde() {
    analogWrite(LED_VERDE_, HIGH_PWM);
    playtone(1136,wait);             
    digitalWrite(LED_VERDE_, LOW);
  }
  
  
 
 // Funcion para mostrar que botón se tenia que pulsar en caso de error del jugador
  void mostrar_boton_correcto(long led) {
    switch (led) {
        case 0:
          flash_rojo();
          break;
        case 1:
          flash_verde();
          break;
        case 2:
          flash_amarillo();
          break;
        case 3:
          flash_azul();
          break;
      }
      delay(50);
  }
 
 
 
 // Función que reproduce la canción al arrancar el arduino y para el juego cuando se llega a la puntuacion maxima
  void felicitacion() {
    // Encedemos todos los led
    digitalWrite(LED_ROJO_, HIGH);       
    digitalWrite(LED_VERDE_, HIGH);
    digitalWrite(LED_AMARILLO_, HIGH);
    digitalWrite(LED_AZUL_, HIGH);

    if (cancion_Mario == 1)
    {
      midi();
      state = 2;
    }
    else
    {
    for (int i = 0; i < length; i++) {
      if (notes[i] == ' ') {
       delay(beats[i] * tempo); // rest
      } else {
       playNote(notes[i], beats[i] * tempo);
      }
      delay(tempo / 2); 
    }
    }

     
    delay(500);   
    // Apagar todos los led
    digitalWrite(LED_ROJO_, LOW);   
    digitalWrite(LED_VERDE_, LOW);
    digitalWrite(LED_AMARILLO_, LOW);
    digitalWrite(LED_AZUL_, LOW);
    resetcontador();    
  }
    
 // Resetear contadores
  void resetcontador() {
    contador = 0;
    wait = 500;
  }
 

// Musica Mario
 void midi() {

    tone(tonePin, 195, 98.2530375);
    delay(109.170041667);
    delay(5.02756770833);
    tone(tonePin, 261, 98.2530375);
    delay(109.170041667);
    delay(5.74579166667);
    tone(tonePin, 329, 97.6066359375);
    delay(108.451817708);
    delay(7.18223958333);
    tone(tonePin, 391, 98.2530375);
    delay(109.170041667);
    delay(5.02756770833);
    tone(tonePin, 523, 98.2530375);
    delay(109.170041667);
    delay(5.74579166667);
    tone(tonePin, 659, 97.6066359375);
    delay(108.451817708);
    delay(7.18223958333);
    tone(tonePin, 783, 294.7591125);
    delay(327.510125);
    delay(17.237375);
    tone(tonePin, 659, 294.7591125);
    delay(327.510125);
    delay(17.237375);
    tone(tonePin, 207, 98.2530375);
    delay(109.170041667);
    delay(5.02756770833);
    tone(tonePin, 261, 98.2530375);
    delay(109.170041667);
    delay(5.74579166667);
    tone(tonePin, 311, 97.6066359375);
    delay(108.451817708);
    delay(7.18223958333);
    tone(tonePin, 415, 98.2530375);
    delay(109.170041667);
    delay(5.02756770833);
    tone(tonePin, 523, 98.2530375);
    delay(109.170041667);
    delay(5.74579166667);
    tone(tonePin, 622, 97.6066359375);
    delay(108.451817708);
    delay(7.18223958333);
    tone(tonePin, 830, 294.7591125);
    delay(327.510125);
    delay(17.237375);
    tone(tonePin, 622, 294.7591125);
    delay(327.510125);
    delay(17.237375);
    tone(tonePin, 233, 98.2530375);
    delay(109.170041667);
    delay(5.02756770833);
    tone(tonePin, 293, 98.2530375);
    delay(109.170041667);
    delay(5.74579166667);
    tone(tonePin, 349, 97.6066359375);
    delay(108.451817708);
    delay(7.18223958333);
    tone(tonePin, 466, 98.2530375);
    delay(109.170041667);
    delay(5.02756770833);
    tone(tonePin, 587, 98.2530375);
    delay(109.170041667);
    delay(5.74579166667);
    tone(tonePin, 698, 97.6066359375);
    delay(108.451817708);
    delay(7.18223958333);
    tone(tonePin, 932, 294.7591125);
    delay(327.510125);
    delay(17.237375);
    tone(tonePin, 932, 98.2530375);
    delay(109.170041667);
    delay(5.02756770833);
    tone(tonePin, 932, 98.2530375);
    delay(109.170041667);
    delay(5.74579166667);
    tone(tonePin, 932, 97.6066359375);
    delay(108.451817708);
    delay(7.18223958333);
    tone(tonePin, 659, 294.7591125);
    delay(327.510125);
    noTone(tonePin);
 }
  
  
 // Funcion para crear y reproducir los patrones
  void mostrar_secuencia() {
 
    randomSeed(analogRead(8));    // Semilla para que la función Random sea más aleatoria

    sequence[contador] = random(4);   // random(4) nos dará un valor aleatorio entre 0 y 3 (0,1,2,3)
               
    for (int i = 0; i < contador; i++) {  
      mostrar_boton_correcto(sequence[i]);             
    }
    wait = 500 - (contador * 15);        
    contador++;                          
  }
 

 
  // Funcion para leer los botones que pulsa el jugador
 void leer_secuencia() {
   for (int i=1; i < contador; i++) {    
      while (input==5){                
        if (digitalRead(BOTON_ROJO_) == LOW) {
          input = 0;
          Serial.println("boton_rojo_leido");
        }
        else if (digitalRead(BOTON_VERDE_) == LOW) { 
          input = 1;
          Serial.println("boton_verde_leido");
        }
        else if (digitalRead(BOTON_AMARILLO_) == LOW) {
          input = 2;
          Serial.println("boton_amarillo_leido");
        }
        else if (digitalRead(BOTON_AZUL_) == LOW) {
          input = 3;
          Serial.println("boton_azul_leido");
        }
      }// while
    
      if (sequence[i-1] == input) {             
        mostrar_boton_correcto(input);
        aciertos = i;
        if (aciertos_por_partida < i) {
        aciertos_por_partida = i;}                          
        if (i == puntuacion_maxima) {
          cancion_Mario = 1;                        
          felicitacion();                        
        }
      }
      else {
          playtone(4545,1500);                  
          delay(500);
          mostrar_boton_correcto(sequence[i-1]);                 
          mostrar_boton_correcto(sequence[i-1]);                 
          mostrar_boton_correcto(sequence[i-1]);
          delay(1000);
          aciertos = 0;
          actualiza = 1;
          felicitacion();
          resetcontador();                          
      } 
      input = 5;                                   
    }// for
  } // void
