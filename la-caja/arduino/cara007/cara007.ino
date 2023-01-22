/* Librería general de la caja  */
#include <Arduino.h>
#include <infra.h>
/* #include <ArduinoJson.h> */


#define LDR_PIN A0 // Pin for the LDR sensor

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

#define D3_GPIO00_FLASH     A0
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
  int pinnum; // pin al que está conectado el botón
  int pinmod; // modo del botón (binario o analógico)
  int thresh             = 128; // threshold para HIGH/LOW en caso de entrada analógica

  unsigned int ms00s0_      = 0;  // tiempo del flanco anterior de bajada (letra a)
  unsigned int ms01s1_      = 0;  // tiempo del flanco de subida en el signo anterior
  unsigned int ms10s1_      = 0;  // tiempo del flanco de bajada en el signo anterior
  unsigned int ms01s2_      = 0;  // tiempo del flanco de subida en el signo actual
  unsigned int ms10s2_      = 0;  // tiempo del flanco de bajada en el signo actual
  unsigned int prevdurs_    = 0;  // duración de la señal anterior
  unsigned int currdurs_    = 0;  // duración de la señal actual
  unsigned int prevduri_    = 0;  // duración del intervalo anterior
  unsigned int currduri_    = 0;  // duración del intervalo actual
    
  int currvalu_ ;
  int nextvalu ;
  unsigned int msTransit  = 0;
  int iSymbol_, iPhrase_;

  void getNextValueForShip() {
    /* if (pinmod == IIBUTTON_MODE_BINARY) { */
    /*   nextvalu = digitalRead(pinnum); */
    /* } else { */
    nextvalu = analogRead(A0);
    nextvalu = (nextvalu < thresh) ? LOW : HIGH;
    /* } */
  }
  
 public:
  char prevsign             = IIBUTTON_SIGN_INIT; // clasificación de la duración del último intervalo I=inicial, s=signo, L=letter, W=word
  char currsign             = IIBUTTON_SIGN_INIT; // ídem del signo actual
  char previntv             = IIBUTTON_INTV_INIT; // clasificación de la duración del último intervalo I=inicial, s=signo, L=letter, W=word
  char currintv             = IIBUTTON_INTV_INIT; // ídem del signo actual
  char strSymbols[40]; // W...L---L...W
  char strPhrase[10];  // SOS SE ACABO

      
  // inicializa el botón y el modo digital o analógico;
  void Setup(int pinNum=LDR_PIN, int pinMod = 2) { 
    //--- Inicializaciones específicas del actuador
    pinMode(LDR_PIN,INPUT);
    pinnum = pinNum;
    pinmod = pinMod;
    getNextValueForShip();
    currvalu_ = nextvalu;      
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
  int HasChanged(void) {
    int serialCount;
    unsigned int now = millis();
    /* getNextValueForShip(); */
    if (serialCount == 10000) {
      nextvalu = analogRead(A0);
      serialCount = 0;
    } else {
      serialCount ++;
    }
          
    if(nextvalu < thresh){
      /* nextvalu = LOW; */
      nextvalu = 0;
    }else{
      /* nextvalu = HIGH; */
      nextvalu = 1;
    }
          
    if (currvalu_ == nextvalu) // aún no ha pasado nada, esperamos a una transición
      msTransit = now;
    else if (now - msTransit < 30) // aún no han pasado 100 milisegundos en el nuevo estado, esperar
      Serial.println("Ricardo ;");
    else { // se confirma la transición después de 100 ms
      if (currvalu_ == HIGH && nextvalu== LOW) { // flanco positivo, se ha pulsado el botón (REVISAR)
        currvalu_ = LOW;
        ms01s1_ = ms01s2_;
        ms01s2_ = msTransit;
      }
      else if (currvalu_ == LOW && nextvalu ==HIGH){
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
              
              
        return 1;
      }
    }
    return 0;
  }         
  // HasChanged
       


  
};
IIButton objFlash; 

#endif  

//--------------------------------------------------------------------------- TM1637



/* TODO Estandarizar los nombres de topics en la infrasestructura */
/* Ejemplo de topic que cumple: #define TOPIC_PUB_ "II3/ESP14440037/resultados_juego2" */
Infra objInfra;
char *strTopicPub = "II3/ESP006/pub/cara006"; /* topic principal para publicar contenido y lastwill */
char *strTopicCmd = "II3/ESP006/cmd/cara006"; /* topic para recibir peticiones de comando */



void setup(){
  /* Setup de la infraestructura */
  pinMode(LDR_PIN,INPUT);
  objFlash.Setup(LDR_PIN,IIBUTTON_MODE_ANALOG);     
  objInfra.mqttTopicsPub[TOPIC_MAIN] = strTopicPub;
  objInfra.mqttTopicsSub[TOPIC_NUM_CMD] = strTopicCmd;
  objInfra.Setup(mqttCallback);
}

int ans = NULL; /* Resupuesta del acertijo */

char *GameAns = "SOS"; // String Inicialization

void mqttCallback(char* topic, byte* payload, unsigned int length){
  /* Procesa los mensajes enviados por Node-red */
  char *mensaje = (char *)malloc(length+1);
  strncpy(mensaje, (char*)payload, length);
  mensaje[length]='\0';
  if (strcmp(topic, strTopicCmd)==0){
    ans = atoi(mensaje);
    objInfra.ReportStart(NULL);}
  free(mensaje);
}

static char strDigits[10]= "";

void loop(){

  int bchgFlash = objFlash.HasChanged();
  static unsigned int msLastReport = millis();
  objInfra.Loop();

  if (!objInfra.GameRunning())
    return;

  if ((millis() > msLastReport + 15000) && (bchgFlash)) {
    msLastReport = millis(); // reseteamos para no enviar más hasta dentro de 15 segundos
  }

  Serial.println(objFlash.strPhrase);
  
  /* if (GameAns == objFlash.strPhrase) */
  /*   { */
  /*     Serial.println("YOU WON"); */
  /*     objInfra.ReportSuccess(" ");  // Cast the const char* to a char* */
  /*   } */

  
  /* else if ((key == CHAR_SEND) && (ans != number)){ */
  /*   /\* si se pulsa enviar y la respuesta es incorrecta *\/ */
  /*   objInfra.ReportFailure(" "); /\* Actualiza el estado a: "Failure" y suma un intento *\/ */
  /*   strcpy(strDigits,"");        /\* Limpio el string en Node-Red *\/ */
  /*   number = 0; /\* Reinicio el número *\/ */
  /* } */
  /* else if (key >= CHAR_0 && key <= CHAR_9){ */
  /*   /\* Envía el número a Node-Red *\/ */
  /*   if(countDigit(number) < MAX_DIGITS){ */
  /*     /\* Evita el overflow *\/ */
  /*     number = 10*number + (key-48); /\* Concateno dígito a digíto para formar un número *\/ */
  /*   } */
  /*   String str = String(number); */
  /*   str.toCharArray(strDigits, 10); */
  /*   objInfra.ReportStatus2(strDigits); */
  /* }   */

}
