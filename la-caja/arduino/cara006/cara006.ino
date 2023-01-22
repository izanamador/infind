#include <Arduino.h>
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
        static int serialCount = 0;
        if (pinmod_ == IIBUTTON_MODE_BINARY) {
          nextvalu_ = digitalRead(pinnum_);
        } else {
          if (serialCount == 100) {
            nextvalu_ = analogRead(pinnum_);
            nextvalu_ = (nextvalu_ < thresh_) ? LOW : HIGH;
            serialCount = 0;
          } else {
            serialCount ++;
          }
          
        }
      }

//void getNextValu() {
//        if (pinmod_ == IIBUTTON_MODE_BINARY) {
//          nextvalu_ = digitalRead(pinnum_);
//        } else {
//          nextvalu_ = analogRead(pinnum_);
//          nextvalu_ = (nextvalu_ < thresh_) ? LOW : HIGH;
//        }
//      }
//  
    public:
      char prevsign             = IIBUTTON_SIGN_INIT; // clasificación de la duración del último intervalo I=inicial, s=signo, L=letter, W=word
      char currsign             = IIBUTTON_SIGN_INIT; // ídem del signo actual
      char previntv             = IIBUTTON_INTV_INIT; // clasificación de la duración del último intervalo I=inicial, s=signo, L=letter, W=word
      char currintv             = IIBUTTON_INTV_INIT; // ídem del signo actual
      char strSymbols[40]; // W...L---L...W
      char strPhrase[3];  // SOS SE ACABO

      
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

/*******************************************************
The player has to send the message SOS with morse code.
If the LDR is light up close by a phone light will send a dot.
If not, it will send a dash.

If the player clicks the button it will add the letter of the morse code to the message
To reset the message the player must pulse the button for a longer time.

SOS is ...---...
********************************************************/

/****************************/
/* Includes and definitions */
/****************************/

// Include Libraries
#include <infra.h>
#include <ezButton.h>

// Infra setup
Infra objInfra;
char *strTopicPub = "II3/ESP006/pub/cara006"; // topic principal para publicar contenido y lastwill
char *strTopicCmd = "II3/ESP006/cmd/cara006"; // topic para recibir peticiones de comando

// Pin Definitions
#define LDR_PIN A0 // Pin for the LDR sensor
#define PUSHBUTTON_PIN 15

ezButton button(PUSHBUTTON_PIN);  

// String Inicialization
char GameAns[3] = {'S', 'O', 'S'};


/****************************/
/*          Game            */
/****************************/
void setup()
{
  objFlash.Setup(A0,IIBUTTON_MODE_ANALOG);     
  objInfra.mqttTopicsPub[TOPIC_MAIN] = strTopicPub;
  objInfra.mqttTopicsSub[TOPIC_NUM_CMD] = strTopicCmd;
  objInfra.Setup(mqttCallback);

  button.setDebounceTime(100);
}

void mqttCallback(char* topic, byte* payload, unsigned int length){
  /* Procesa los mensajes enviados por Node-red */
  char *mensaje = (char *)malloc(length+1);
  strncpy(mensaje, (char*)payload, length);
  mensaje[length]='\0';
  if (strcmp(topic, strTopicCmd)==0){
    objInfra.ReportStart(NULL);}
  free(mensaje);
}

 static char strDigits[10]= "";
 
void loop()
{
   button.loop();
  bool bchgFlash = objFlash.HasChanged();
   static unsigned int msLastReport = millis(); 
  
  objInfra.Loop();
  if (!objInfra.GameRunning())
    return;


  // comprobamos si han pasado los 15 segundos desde la última vez
     if ((millis() > msLastReport + 1000) && (bchgFlash)) { 
       msLastReport = millis(); // reseteamos para no enviar más hasta dentro de 15 segundos 
     } 

if (button.isReleased() && (strcmp(GameAns, objFlash.strPhrase) == 0)){
    objInfra.ReportSuccess(" "); 
  }
  else if (button.isReleased()){
    objInfra.ReportFailure(" "); /* Actualiza el estado a: "Failure" y suma un intento */
    objFlash.Reset();
    /* LIMPIA EL STRING */
    objInfra.ReportStatus2(objFlash.strPhrase); /* Por si las dudas */
  }
  else if (bchgFlash){
    objInfra.ReportStatus2(objFlash.strPhrase);
  }
}
   
