#include "IIHardware.h"

#include "IIArch.h"
IIWifi  obWifi;
IIMqtt  obMqtt;
IIDht11 obDht11;


//------ librerías utilizadas
  //#include <ESP8266WiFi.h>   // WIFI
  //#include <PubSubClient.h>  // MQTT
  #include "ArduinoJson.h"   // JSON
  

//------- parámetros de configuración constantes (en MAYUSCULAS)
  //const char* WIFI_SSID       = "infind";
  //const char* WIFI_PASS       = "1518wifi";
  // const char* MQTT_SERVER     = "iot.ac.uma.es";
  // const char* MQTT_USER       = "II3"; // "infind";
  // const char* MQTT_PASS       = "qW30SImD"; // "zancudo";
  // const int   MQTT_MAX_BUFFER = 512;

void decodeMorse(char* strSymbols, char *strPhrase)
{
  // Morse code lookup table
  const char morseTable[26][6] = {
      ".- ",   // A
      "-... ", // B
      "-.-. ", // C
      "-.. ",  // D
      ". ",    // E
      "..-. ", // F
      "--. ",  // G
      ".... ", // H
      ".. ",   // I
      ".--- ", // J
      "-.- ",  // K
      ".-.. ", // L
      "-- ",   // M
      "-. ",   // N
      "--- ",  // O
      ".--. ", // P
      "--.- ", // Q
      ".-. ",  // R
      "... ",  // S
      "- ",    // T
      "..- ",  // U
      "...- ", // V
      ".-- ",  // W
      "-..- ", // X
      "-.-- ", // Y
      "--.. "  // Z
  };

  int iPositionIn = 0;
  int iPositionOut = 0;
  int j;
  char strSymbolsEsp [100];
  sprintf(strSymbolsEsp, "%s ", strSymbols);
  Serial.printf("--------->>>>>>>>>> Decodificando %s\n", strSymbols);
  strcpy(strSymbols, strSymbolsEsp);

  while (true) {
    Serial.printf("Buscando en %s\n", (char*)(strSymbols+iPositionIn));
    if (strSymbols[iPositionIn]=='\0') break;
    if (strSymbols[iPositionIn]==' ') {iPositionIn++;continue;}
    for (j=0; j<26; j++) {
      if (strncmp(strSymbols+iPositionIn, morseTable[j], strlen(morseTable[j]))==0) {
        Serial.printf("%s=%c\t", morseTable[j], (char)('A'+j));
        strPhrase[iPositionOut++] = (char)('A'+j);
        iPositionIn += strlen(morseTable[j]);
        break;
      }
    }
    if (j>=26)  {
      strPhrase[iPositionOut++] = '?';
      
      while ((strSymbols[iPositionIn] != ' ') && (strSymbols[iPositionIn] != '\0')) {
        Serial.printf("%c", strSymbols[iPositionIn]);
        iPositionIn++;
      }
      Serial.printf("=?\t");        
    }

    if (strSymbols[iPositionIn] == '\0') {
      strPhrase[iPositionOut] = '\0';
      Serial.printf("\n---------<<<<<<<<<<[%s] = [%s]\n", strSymbols, strPhrase);
      return;
    }
    else if (strSymbols[iPositionIn] ==' ')
      iPositionIn++;
  }
}


//--------------------------------------------------------------------------- TM1637
  #ifndef IIFLASH_
  #define IIFLASH_

  #define D3_GPIO00_FLASH     0
  #define IIBUTTON_MODE_BINARY 1
  #define IIBUTTON_MODE_ANALOG 2

  #define IIBUTTON_INTV_INIT    'I'  // inicialización
  #define IIBUTTON_INTV_SIGNAL  's'  // sin actividad (desde últ bajada a subida) entre 100 y 500 ms
  #define IIBUTTON_INTV_LETTER  'L'  // sin actividad entre 501 y 1000 ms
  #define IIBUTTON_INTV_WORD    'W'  // sin actividad por más de 1000 ms
  
  #define IIBUTTON_SIGN_INIT    'I'     // activo entre 100 y 500 ms
  #define IIBUTTON_SIGN_DOT     '.'     // activo entre 100 y 500 ms
  #define IIBUTTON_SIGN_DASH    '-'    // activo entre 501 y 1000 ms
  #define IIBUTTON_SIGN_LONG    '_'    // activo por más de 1000 ms

  class IIButton {
    private:
      int pinnum_; // pin al que está conectado el botón
      int pinmod_; // modo del botón (binario o analógico)
      int thresh_             = 128; // threshold para HIGH/LOW en caso de entrada analógica

      unsigned int ms00s0_      = 0;  // tiempo del flanco anterior de bajada (letra a)
      unsigned int ms01s1_      = 0;  // tiempo del flanco de subida en el signo anterior
      unsigned int ms10s1_      = 0;  // tiempo del flanco de bajada en el signo anterior
      unsigned int ms01s2_      = 0;  // tiempo del flanco de subida en el signo actual
      unsigned int ms10s2_      = 0;  // tiempo del flanco de bajada en el signo actual
      unsigned int prevdurs_    = 0;  // duración de la señal anterior
      unsigned int currdurs_    = 0;  // duración de la señal actual
      unsigned int prevduri_    = 0;  // duración del intervalo anterior
      unsigned int currduri_    = 0;  // duración del intervalo actual
    
      int currvalu_ = HIGH;
      int nextvalu_ = HIGH;
      unsigned int msTransit  = 0;
      int iSymbol_, iPhrase_;

      void getNextValu() {
        if (pinmod_ == IIBUTTON_MODE_BINARY) {
          nextvalu_ = digitalRead(pinnum_);
        } else {
          nextvalu_ = analogRead(pinnum_);
          nextvalu_ = (nextvalu_ < thresh_) ? LOW : HIGH;
        }
      }
  
    public:
      char prevsign             = IIBUTTON_SIGN_INIT; // clasificación de la duración del último intervalo I=inicial, s=signo, L=letter, W=word
      char currsign             = IIBUTTON_SIGN_INIT; // ídem del signo actual
      char previntv             = IIBUTTON_INTV_INIT; // clasificación de la duración del último intervalo I=inicial, s=signo, L=letter, W=word
      char currintv             = IIBUTTON_INTV_INIT; // ídem del signo actual
      char strSymbols[40]; // W...L---L...W
      char strPhrase[10];  // SOS SE ACABO

      
      // inicializa el botón y el modo digital o analógico;
        void Setup(int pinNum=D3_GPIO00_FLASH, int pinMod = IIBUTTON_MODE_BINARY) { 
          //--- Inicializaciones específicas del actuador
            pinnum_ = pinNum;
            pinmod_ = pinMod;
            getNextValu();
            currvalu_ = nextvalu_;      
            iSymbol_ = 0;
            iPhrase_ = 0;
            strSymbols[iSymbol_] = '\0';
            strPhrase[iPhrase_] = '\0';
        }         
      // Setup
      void Reset() {
        iSymbol_ = 0;
            iPhrase_ = 0;
            strSymbols[iSymbol_] = '\0';
            strPhrase[iPhrase_] = '\0';                
      }
      
      // Comprueba si ha habido un flanco de subida o bajada
        bool HasChanged(void) {         
          unsigned int now = millis();
          getNextValu();
                      
          if (currvalu_ == nextvalu_) // aún no ha pasado nada, esperamos a una transición
            msTransit = now;
          else if (now - msTransit < 30) // aún no han pasado 100 milisegundos en el nuevo estado, esperar
            ;
          else { // se confirma la transición después de 100 ms
            if (currvalu_ == HIGH && nextvalu_== LOW) { // flanco positivo, se ha pulsado el botón (REVISAR)
              currvalu_ = LOW;
              ms01s1_ = ms01s2_;
              ms01s2_ = msTransit;              
            }
            else if (currvalu_ == LOW && nextvalu_==HIGH){
              currvalu_  = HIGH;
              ms00s0_    = ms10s1_;
              ms10s1_    = ms10s2_;
              ms10s2_    = msTransit;
              // shift de valores curr a valores prev
              prevsign   = currsign;
              previntv   = currintv;
              prevdurs_  = currdurs_;
              prevduri_  = currduri_;
              // cálculo de valores current              
              currdurs_  = ms10s2_ - ms01s2_; // flanco bajada s2 - flanco subida s2
              currduri_  = ms01s2_ - ms10s1_; // flanco subida s2 - flanco bajada s1
              currsign   = (currdurs_ < 300) ? IIBUTTON_SIGN_DOT    : ((currdurs_ < 1500) ? IIBUTTON_SIGN_DASH   : IIBUTTON_SIGN_LONG );              
              currintv   = (currduri_ < 300) ? IIBUTTON_INTV_SIGNAL : ((currduri_ < 1500) ? IIBUTTON_INTV_LETTER : IIBUTTON_INTV_WORD );
              Serial.printf("%c%c ", currintv, currsign);
              if (currintv==IIBUTTON_INTV_WORD) { // poner un espacio antes si estamos empezando una palabra
                strSymbols[iSymbol_++] = ' ';
                strSymbols[iSymbol_+1] = '\0';
              }
              if (currsign==IIBUTTON_SIGN_DOT) {
                strSymbols[iSymbol_++] = '.';
                strSymbols[iSymbol_+1] = '\0';
              }                
              else if (currsign==IIBUTTON_SIGN_DASH) {
                strSymbols[iSymbol_++] = '-';
                strSymbols[iSymbol_+1] = '\0';
              }
              Serial.println(strSymbols);
              decodeMorse(strSymbols, strPhrase);
              Serial.println(strPhrase);
              
              
              return true;
            }
          }
          return false;
        }         
       // HasChanged
       


  
  };
  IIButton objFlash; 

  #endif  

//--------------------------------------------------------------------------- TM1637


//------- variables globales (empiezan en mayúsculas)
  char EspId[20];
  //WiFiClient WifiClient;                        // cliente wifi necesario para el constructor de PubSubClient
  //PubSubClient Mqtt_client(obWifi.Client);         // cliente mqtt
  //int Reintentos = 0;
  //char Topic_Pub[100], Topic_Sub[100];



//------- función para conectarse a la Wifi
// void conecta_wifi() {
//     // Comment("conecta_wifi", 1, "inicialización a partir de los parámetros de conexión");
//       Serial.printf("\nConectando a la Wifi %s:\n", WIFI_SSID); 
//       WiFi.mode(WIFI_STA);                    // WiFi y WIFI_STA vienen de la librería
//       WiFi.begin(WIFI_SSID, WIFI_PASS);  

//   //--- reintento de conexión cada 0.2 segundos
//     while (WiFi.status() != WL_CONNECTED) {   // WL_CONNECTED también viene de la librería
//       delay(200);                             // espera bloqueante!!!
//       Serial.print(".");                      // feedback al usuario
    
//       if (Reintentos++ % 100 == 0) {        
//         Serial.printf("\nReintento número %02d : ", Reintentos);
//       }
//     }

//   //--- conexión WiFi disponible para MQTT
//     Serial.printf("\nConectado a Wifi %s con éxito.\n Dirección IP: %s\n",  WIFI_SSID, WiFi.localIP().toString().c_str());

// }


//------- función para conectar o reconectar a MQTT y suscribirse/resuscribirse a los topics y recibir mensajes mqtt 
// void mqtt_con_sub() {
  
//   //Comment("mqtt_con_sub", 1, "inicio");

//   //--- antes de cada envío hay que comprobar que no se cayó la conexión y reintentar recuperarla
//     Reintentos = 0; // resetea el contador de reintentos de conexión

//   //--- mientras no haya  conexión hay que reintentar conectar las veces que hagan falta

//     while (!Mqtt_client.connected()) {
      
//         // damos feedback del estado de la conexión y reintentamos conectar
//           Serial.printf("\n\nIntento %d de conexión al servidor MQTT %s\n", ++Reintentos, MQTT_SERVER);

//         // Comment("mqtt_con_sub", 22, "sólo podemos suscribirnos a topics cuando tenemos una conexión válida");
//           if (Mqtt_client.connect(EspId, MQTT_USER, MQTT_PASS)) {

//             //Comment("mqtt_con_sub", 23, "si la conexión fue bien ya se pueden hacer suscripciones");
//             Serial.printf("Conexión al broker %s con éxito.\n Suscribiendo a topics: %s\n", MQTT_SERVER, Topic_Sub);

//             // nos suscribimos al mismo topic por el que publicamos para cambiar el led cada vez que publiquemos
//             Mqtt_client.subscribe(Topic_Sub);
//             //Comment("mqtt_con_sub", 24, "Topic_Sub");

//           }
//         //Comment("mqtt_con_sub", 2, "sólo podemos suscribirnos a topics cuando tenemos una conexión válida");
//         //--- si la conexión falló hay que visualizar el mensaje de error (rc)
//           else {
//             Serial.printf("Conexión fallida. Error de salida rc=%d. Reintento dentro de 5 segundos\n", Mqtt_client.state());
//             delay(5000); // espera bloqueante durante 5 segundos antes de reintentar
//           }
//       } 

// }

//------- función que se invocará cada vez que llegue un mensaje 
void procesa_mensaje(char* strTopicMsg, byte* payload, unsigned int length) {
  bool mrMqtt = obMqtt.Message(strTopicMsg, payload, length);
  // // Comment("procesa_mensaje", 25, "inicio");


  // //--- copio el mensaje recibido (para que no pete) y le pongo un nulo al final
  //   char *mensaje = (char *)malloc(length+1); // reservo memoria para copia del mensaje
  //   strncpy(mensaje, (char*)payload, length); // copio el mensaje en cadena de caracteres
  //   mensaje[length]='\0'; // caracter cero marca el final de la cadena
  //   Serial.printf("Mensaje recibido desde el topic [%s]\n---------%s\n\n", Topic_Sub, mensaje);

  // //--- compruebo de qué topic vino para decidir qué hacer con él
  //   if(strcmp(strTopicMsg, Topic_Sub)==0)  {
  //     // por ejemplo, apago/enciendo el led dependiendo del primer byte del mensaje
  //     // digitalWrite(D4_GPIO02_TXD1_LED1, (mensaje[0]=='1'? LOW : HIGH)); 
  //     ;
  //   }

  // //--- antes de salir hay que liberar la memoria              
  //   free(mensaje);
  
}

//------- función que se invocará una sola vez al inicio del sketch
void setup() {
  //--- informar de que la placa ha arrancado
    Serial.begin(115200);
    Serial.println();
    Serial.println("Empieza setup...");
    obWifi.Setup();
    obMqtt.Setup(&obWifi, procesa_mensaje);
    obDht11.Setup(&obMqtt, D1_GPIO05_SDA, 30); // medir por el pin SDA cada 30 segundos

    //obDht11.Setup(-D1_GPIO05_SDA); // si no hay sensor comentar la anterior y dejar esta

  
  //Comment("setup", 1, "Inicialización de variables globales");
    //--------------------------------------------------------------------------
    //sprintf(EspId, "ESP_%d", ESP.getChipId());
    //sprintf(Topic_Pub, "II3/GAME/STATUS/chrono/ESP%d", ESP.getChipId()); // ejemplo examen/ejercicio2/ESP1126590 ,
    //sprintf(Topic_Sub, "II3/GAME/COMMAND/ALL", ESP.getChipId()); // ejemplo examen/ejercicio2/ESP1126590 ,
    ///strcpy(Topic_Sub, Topic_Pub); // nos auto-suscribimos

  //Comment("setup", 2, "Imprimimos la configuración");
      // Serial.printf("Identificador placa: %s\n", EspId );
      // Serial.printf("Topic publicacion  : %s\n", Topic_Pub);
      // Serial.printf("Topic subscripcion : %s\n", Topic_Sub);
      // Serial.printf("Termina setup en %lu ms\n\n",millis());

  //Comment("setup", 3, "Conectamos a la wifi");
    //--- Conectamos a la wifi    
      // conecta_wifi();

    //Comment("setup", 3, "Y preparamos las conexiones a Mqtt a partir de parámetros de las primeras líneas");
    //  Mqtt_client.setServer(MQTT_SERVER, 1883);
    //  Mqtt_client.setBufferSize(MQTT_MAX_BUFFER); 
    //  Mqtt_client.setCallback(procesa_mensaje);
      //mqtt_con_sub_loop();
      // mqtt_con_sub();
    
  //--- inicialización del hardware específico montado en el ESP
    objFlash.Setup();     
 
}


//------- función principal que que se invoca periódicamente
void loop() {  
  bool hcWifi  = obWifi.HasChanged();
  bool hcMqtt  = obMqtt.HasChanged();
  bool hcDht11 = obDht11.HasChanged();

  //--- si ya había conexión o se ha reconectado invocamos a loop para recibir posibles mensajes
    //mqtt_con_sub();
    //Mqtt_client.loop(); 
    bool bchgFlash = objFlash.HasChanged();
    static unsigned int msLastMessage = millis();    
    unsigned int now = millis();
    if (bchgFlash) msLastMessage = millis();
    if (now > msLastMessage+20000)
      objFlash.Reset(); 
    
  //   static unsigned int msLastReport = millis();

  // // comprobamos si han pasado los 15 segundos desde la última vez
  //   if ((millis() > msLastReport + 5000)) {// } && (bchgFlash)) {
  //     msLastReport = millis(); // reseteamos para no enviar más hasta dentro de 15 segundos

  //     //--- construimos el documento sobre el que vamos a reportar
  //       StaticJsonDocument<300> doc;
  //       doc["esp"]["uptime"]        = millis(); // "2509853";
  //       doc["esp"]["vcv"]           = ESP.getVcc()/1000; // batería de alimentación en voltios
  //       doc["wifi"]["ssid"]         = obWifi.Ssid; 
  //       doc["wifi"]["rssi"]         = obWifi.Rssi;
  //       doc["wifi"]["ip"]           = obWifi.Ip;
  //       doc["dht11"]["sda"]         = obDht11.getSda();
  //       doc["dht11"]["temp"]        = obDht11.getTemperature();
  //       doc["dht11"]["hum"]         = obDht11.getHumidity();

        
  //     // imprimimos el mensaje formateado para que sea más fácil de ver
  //       char strSerialized[1000];
  //       serializeJsonPretty(doc, strSerialized);
  //       Serial.printf("Enviando por %s\n---------\n%s\n---------\n", Topic_Pub, strSerialized);
  //       Serial.println();

  //     // serializamos sin formato para que ocupe menos al enviarlo por la wifi
  //       serializeJson(doc,strSerialized);
  //       //obMqtt.publish(Topic_Pub, strSerialized);
  //       doc.clear();
        
  //   }
}
