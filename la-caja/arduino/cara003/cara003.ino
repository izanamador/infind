// Infraestructura común de la caja
#include <infra.h>
char *strTopicPub = "II3/ESP14440037/resultados_juego2"; // topic principal para publicar contenido y lastwill
char *strTopicCfg = "II3/ESP002/cfg/cara002"; // topic para recibir parametros de configuracion
char *strTopicCmd = "II3/ESP14440037/datos_juego2"; // topic para recibir peticiones de comando

// Variables globales
Infra objInfra;
#define MAX_SEQ   5 // TODO: EL NUMERO DE AYDUAS ES MAX_SEQ-LongitudSecuencia
int LongitudSecuencia=MAX_SEQ;  // número de teclas que tiene que acertar
int SecuenciaMasLarga=0;        // para reportarlo a nodered


int SecuenciaCorrecta[20]; // secuencia de colores cada vez más larga
#define COLOR_ROJO      0
#define COLOR_VERDE     1
#define COLOR_AMARILLO  2
#define COLOR_AZUL      3
#define COLOR_NINGUNO   4
  

// Librerías específicas del juego
#include <ArduinoJson.h>

  

// Pines de salida 
#define PIN_LED_ROJO      5       // D1(serigrafiado)-GPIO5      
#define PIN_LED_AZUL      4       // D2-GPIO4 
#define PIN_LED_AMARILLO  2       // D4-GPIO2
#define PIN_LED_VERDE     14      // D5-GPIO14
#define PIN_TONO          12      // D6-GPIO12
 

// Pines de entrada para los botones
#define PIN_BOTON_ROJO     13      // D7-GPIO13
#define PIN_BOTON_AZUL     16      // D0-GPIO16  // ojo, el 15 no va
#define PIN_BOTON_AMARILLO 10      // SSD3-GPIO10
#define PIN_BOTON_VERDE    0       // D3-GPIO0   // ojo, SSD2-9 no lee bien



//********************************
// Setup
//********************************
void setup() {
 // Configuración de los pines de los leds y del zumbador como salidas
    pinMode(PIN_LED_ROJO,     OUTPUT);      
    pinMode(PIN_LED_AZUL,     OUTPUT); 
    pinMode(PIN_LED_AMARILLO, OUTPUT); 
    pinMode(PIN_LED_VERDE,    OUTPUT); 
    pinMode(PIN_TONO,         OUTPUT);
      
 // Configuración de los pines de los botones como entradas
    pinMode(PIN_BOTON_ROJO,     INPUT);    
    pinMode(PIN_BOTON_AZUL,     INPUT);
    pinMode(PIN_BOTON_AMARILLO, INPUT);
    pinMode(PIN_BOTON_VERDE,    INPUT);
 
  // Melodia de inicio al arrancar el Arduino    
  //   melodia_inicio();
  //   resetcontador();    

  // Inicialización de la infraestructura  
    objInfra.mqttTopicsPub[TOPIC_MAIN] = strTopicPub;
    objInfra.mqttTopicsSub[TOPIC_NUM_CFG] = strTopicCfg;
    objInfra.mqttTopicsSub[TOPIC_NUM_CMD] = strTopicCmd;  
    objInfra.Setup(procesa_mensaje);
  }



//********************************
// Recepción mensaje
//********************************
#define MESSAGE_SIZE_ 300

void procesa_mensaje(char* topic, byte* payload, unsigned int length) 
{
  StaticJsonDocument<MESSAGE_SIZE_> json_recibido;
  int reducir_secuencia = 0;        // Variable que reduce la puntuación 
  char *mensaje_recibido = (char *)malloc(length+1);
 
  strncpy(mensaje_recibido, (char*)payload, length);    // copio el mensaje_recibido en cadena de caracteres
  mensaje_recibido[length]='\0';                        // caracter cero marca el final de la cadena
  Serial.print("Mensaje recibido"); 
  Serial.println(mensaje_recibido);

  if(strcmp(topic, strTopicCmd)==0)
  {
      deserializeJson(json_recibido,mensaje_recibido);  // leo de JSON
      // state = json_recibido["state"]; 
      reducir_secuencia = json_recibido["reducir_secuencia"];

      // inicializamos contadores con el primer mensaje recibido
      if (reducir_secuencia==0)
        objInfra.ReportStart(NULL); 

      // nos piden ayuda y reducimos la longitud de la secuencia
      else
        LongitudSecuencia -= reducir_secuencia; 
  }     
  json_recibido.clear();                                // limpiar buffer
  free(mensaje_recibido);                               // liberar buffer
}

//********************************
// Programa principal
//********************************

#define STAT_INICIAL    0 // estado inicial
#define STAT_SECUENCIA  1 // alargar y reproducir la secuencia
#define STAT_ESPERA     2 // esperar que el usuario pulse un botón
#define STAT_BOTON      3 // el usuario ha pulsado un botón
#define STAT_BOTON_OK   4 // el usuario pulsó el botón correcto
#define STAT_BOTON_KO   5 // el usuario pulsó el botón erróneo
#define STAT_COMPLETA   6 // el usuario completó la secuencia con éxito
#define MILIS_BOTON_KO  500 // TODO CONFIRMAR QUE EL TIEMPO ES ADECUADO
#define MAX_LUM_PWM     0  // TODO, LA LUMINOSIDAD ESTÁ AL REVES, AL SUBIR EL NÚMERO BAJA LA LUZ
#define MIN_LUM_PWM     200 // TODO, CONFIRMAR QUE CON ESTE VALOR EL LED SE PUEDE VER
void loop() 
{
  // Declaración de variables
    char GameInfo[200]; // TODO REPORTAR EL NUMERO DE AYUDAS Y LONGITUD DE SECUENCIA

    static int estado = STAT_INICIAL;
    static int boton_pulsado;
    static int iBoton;
    static int longParcial;
    static int duracion_sonido;
    static int luminosidad;
    static int ayudasSolicitadas;
    static int aciertosPorPartida;



  // Invocación a la infraestructura y salida si el juego no está activo
    objInfra.Loop("");
    if (!objInfra.GameRunning())
      return;
      if (estado != STAT_ESPERA)
      {
        ayudasSolicitadas = MAX_SEQ-LongitudSecuencia;
        if (iBoton + 1 == LongitudSecuencia)
        {
          aciertosPorPartida = longParcial;
        }
        else
        {
          aciertosPorPartida = longParcial - 1;
        }
        sprintf(GameInfo, "{Estado: %d, ayudasSolicitadas: %d, aciertosPorPartida: %d, LongitudSecuencia: %d}",
          estado, ayudasSolicitadas, aciertosPorPartida, LongitudSecuencia);
        Serial.println(GameInfo);
      }

  // STAT_INICIAL: Inicialización del juego y melodía de inicio
    if (estado == STAT_INICIAL)
    {
      longParcial = 0;
      iBoton = 0;
      duracion_sonido = 500;
      luminosidad = MIN_LUM_PWM; // empezamos con poca luminosidad y vamos subiendo
      melodia_inicio();
      estado = STAT_SECUENCIA;
    }

  // STAT_SECUENCIA: Crear una secuencia cada vez más larga y reproducirla
    else if (estado == STAT_SECUENCIA) 
    {            
      randomSeed(analogRead(8));  // Semilla para que la función Random sea más aleatoria
      SecuenciaCorrecta[longParcial] = random(4); // valor aleatorio entre 0 y 3 (0,1,2,3)
      longParcial++;
      iBoton = 0;
      for (int i = 0; i < longParcial; i++)   
        mostrar_color(SecuenciaCorrecta[i], duracion_sonido, luminosidad);
      estado = STAT_ESPERA;
    }

  // STAT_ESPERA: Esperar a que el usuario pulse un botón
    else if (estado == STAT_ESPERA) 
    { 
      estado = STAT_BOTON;
      if (digitalRead(PIN_BOTON_ROJO) == LOW)
        {boton_pulsado = COLOR_ROJO;
        delay(50);}
      else if (digitalRead(PIN_BOTON_AZUL) == LOW)
        {boton_pulsado = COLOR_AZUL;
        delay(50);}
      else if (digitalRead(PIN_BOTON_AMARILLO) == LOW)
        {boton_pulsado = COLOR_AMARILLO;
        delay(50);}
      else if (digitalRead(PIN_BOTON_VERDE) == LOW)
        {boton_pulsado = COLOR_VERDE;
        delay(50);}
      else
        estado = STAT_ESPERA;
        //boton_pulsado = COLOR_NINGUNO;
    }

  // STAT_BOTON: Visualizar el botón y decidir si completó, acertó o falló
    else if (estado == STAT_BOTON) 
    { 
      char *strBotones[] = {"rojo", "verde", "amarillo","azul"};
      Serial.println(strBotones[boton_pulsado]);
      if((SecuenciaCorrecta[iBoton] == boton_pulsado) && (iBoton + 1 != LongitudSecuencia))
      {
        mostrar_color(boton_pulsado, duracion_sonido, luminosidad);
        Serial.println(luminosidad);
        delay(300);
      } 
      if (SecuenciaCorrecta[iBoton] != boton_pulsado)
        estado = STAT_BOTON_KO;
      else if (iBoton + 1 == LongitudSecuencia)
        estado = STAT_COMPLETA;
      else if (iBoton + 1 == longParcial)
      {
        if (longParcial > SecuenciaMasLarga)
        {
          SecuenciaMasLarga = longParcial;
        }
        estado = STAT_SECUENCIA;
        iBoton = 0;
      }
      else
        estado = STAT_BOTON_OK;
    }

  // STAT_BOTON_KO: el usuario se equivoca de botón, volver a empezar
    else if (estado == STAT_BOTON_KO)
    {
      playTone(4545,1500);
      delay(500);
      
      
      // mostrar 3 veces a máxima luminosidad el botón que se debería haber pulsado
      mostrar_color(SecuenciaCorrecta[iBoton], duracion_sonido, MAX_LUM_PWM);
      mostrar_color(SecuenciaCorrecta[iBoton], duracion_sonido, MAX_LUM_PWM);
      mostrar_color(SecuenciaCorrecta[iBoton], duracion_sonido, MAX_LUM_PWM);
      
      delay(1000);
      objInfra.ReportFailure(GameInfo);
      estado = STAT_INICIAL;
    }

  // STAT_BOTON_OK: el usuario acierta pero no termina, seguir alargando
    else if (estado == STAT_BOTON_OK)
    {
      
      // a medida que se acierta la secuencia la luminosidad de los leds se incrementa
      luminosidad = round((255/LongitudSecuencia) + ((255*longParcial)/LongitudSecuencia));
      
      // a medida que se acierta la secuencia se acorta la duración de los sonidos
      duracion_sonido -= 15;       
      estado = STAT_ESPERA;
      iBoton++;
    }

  // STAT_COMPLETA: el usuario completa hasta el final y gana el juego
    else if (estado == STAT_COMPLETA)
    {
      mostrar_color(boton_pulsado, duracion_sonido, MAX_LUM_PWM);
      melodia_felicitacion();
      objInfra.ReportSuccess(GameInfo);  
    }

} // setup




// ------------------------------- funciones relacionadas con los leds

void mostrar_color(int color, int luminosidad, int tiempo) 
{
  tiempo = 500;
  switch (color) 
  {
    case COLOR_ROJO:
      analogWrite(PIN_LED_ROJO, luminosidad);
      playTone(2273,tiempo);            
      digitalWrite(PIN_LED_ROJO, LOW);
     break;
    case COLOR_AZUL:
      analogWrite(PIN_LED_AZUL, luminosidad);
      playTone(1700,tiempo);            
      digitalWrite(PIN_LED_AZUL, LOW);
      break;
    case COLOR_AMARILLO:
      analogWrite(PIN_LED_AMARILLO, luminosidad);
      playTone(1275,tiempo);             
      digitalWrite(PIN_LED_AMARILLO, LOW);
      break;
    case COLOR_VERDE:
      analogWrite(PIN_LED_VERDE, luminosidad);
      playTone(1136,tiempo);             
      digitalWrite(PIN_LED_VERDE, LOW);
      break;
  }
  delay(50);
}


//------------------------------- funciones relacionadas con el altavoz


void playTone(int tone, int duration) 
{
  for (long i = 0; i < duration * 1000L; i += tone *2) 
  {
    digitalWrite(PIN_TONO, HIGH);
    delayMicroseconds(tone);
    digitalWrite(PIN_TONO, LOW);
    delayMicroseconds(tone);
  }
}
 
void playNote(char note, int duration) 
{
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };
   
  for (int i = 0; i < 8; i++) 
  {
    if (names[i] == note) 
    {
      playTone(tones[i], duration);
    }
  }
}


void melodia_inicio() 
{
  // declaración de variables para notas y duración de cada nota
    char notas[] = "ccggaagffeeddc "; // melodía de inicio
    int duraciones[] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 4 };
    int length = 15;
    int tempo = 100; // Delay entre notas y milisegundos por unidad de duración

  // Encedemos todos los led
    digitalWrite(PIN_LED_ROJO, HIGH);       
    digitalWrite(PIN_LED_VERDE, HIGH);
    digitalWrite(PIN_LED_AMARILLO, HIGH);
    digitalWrite(PIN_LED_AZUL, HIGH);

  // Reproducimos la melodía nota a nota
    for (int i = 0; notas[i] != ' '; i++) 
    {
      playNote(notas[i], duraciones[i] * tempo);
      delay(duraciones[i] * tempo); 
    }

  // Pausa al final de la melodía y apagado de leds
    delay(600);   
    digitalWrite(PIN_LED_ROJO, LOW);   
    digitalWrite(PIN_LED_VERDE, LOW);
    digitalWrite(PIN_LED_AMARILLO, LOW);
    digitalWrite(PIN_LED_AZUL, LOW);
}


void melodia_felicitacion() 
{
  // Encedemos todos los led
  digitalWrite(PIN_LED_ROJO, HIGH);       
  digitalWrite(PIN_LED_VERDE, HIGH);
  digitalWrite(PIN_LED_AMARILLO, HIGH);
  digitalWrite(PIN_LED_AZUL, HIGH);
  
  tone(PIN_TONO, 195, 98.2530375);
  delay(109.170041667);
  delay(5.02756770833);
  tone(PIN_TONO, 261, 98.2530375);
  delay(109.170041667);
  delay(5.74579166667);
  tone(PIN_TONO, 329, 97.6066359375);
  delay(108.451817708);
  delay(7.18223958333);
  tone(PIN_TONO, 391, 98.2530375);
  delay(109.170041667);
  delay(5.02756770833);
  tone(PIN_TONO, 523, 98.2530375);
  delay(109.170041667);
  delay(5.74579166667);
  tone(PIN_TONO, 659, 97.6066359375);
  delay(108.451817708);
  delay(7.18223958333);
  tone(PIN_TONO, 783, 294.7591125);
  delay(327.510125);
  delay(17.237375);
  tone(PIN_TONO, 659, 294.7591125);
  delay(327.510125);
  delay(17.237375);
  tone(PIN_TONO, 207, 98.2530375);
  delay(109.170041667);
  delay(5.02756770833);
  tone(PIN_TONO, 261, 98.2530375);
  delay(109.170041667);
  delay(5.74579166667);
  tone(PIN_TONO, 311, 97.6066359375);
  delay(108.451817708);
  delay(7.18223958333);
  tone(PIN_TONO, 415, 98.2530375);
  delay(109.170041667);
  delay(5.02756770833);
  tone(PIN_TONO, 523, 98.2530375);
  delay(109.170041667);
  delay(5.74579166667);
  tone(PIN_TONO, 622, 97.6066359375);
  delay(108.451817708);
  delay(7.18223958333);
  tone(PIN_TONO, 830, 294.7591125);
  delay(327.510125);
  delay(17.237375);
  tone(PIN_TONO, 622, 294.7591125);
  delay(327.510125);
  delay(17.237375);
  tone(PIN_TONO, 233, 98.2530375);
  delay(109.170041667);
  delay(5.02756770833);
  tone(PIN_TONO, 293, 98.2530375);
  delay(109.170041667);
  delay(5.74579166667);
  tone(PIN_TONO, 349, 97.6066359375);
  delay(108.451817708);
  delay(7.18223958333);
  tone(PIN_TONO, 466, 98.2530375);
  delay(109.170041667);
  delay(5.02756770833);
  tone(PIN_TONO, 587, 98.2530375);
  delay(109.170041667);
  delay(5.74579166667);
  tone(PIN_TONO, 698, 97.6066359375);
  delay(108.451817708);
  delay(7.18223958333);
  tone(PIN_TONO, 932, 294.7591125);
  delay(327.510125);
  delay(17.237375);
  tone(PIN_TONO, 932, 98.2530375);
  delay(109.170041667);
  delay(5.02756770833);
  tone(PIN_TONO, 932, 98.2530375);
  delay(109.170041667);
  delay(5.74579166667);
  tone(PIN_TONO, 932, 97.6066359375);
  delay(108.451817708);
  delay(7.18223958333);
  tone(PIN_TONO, 659, 294.7591125);
  delay(327.510125);
  noTone(PIN_TONO);

  // Pausa al final de la melodía y apagado de leds
  delay(600);   
  digitalWrite(PIN_LED_ROJO, LOW);   
  digitalWrite(PIN_LED_VERDE, LOW);
  digitalWrite(PIN_LED_AMARILLO, LOW);
  digitalWrite(PIN_LED_AZUL, LOW);
}
 
