//*****************************
// Adicionales
//*****************************
int fin_del_juego = 0;
int cancion_Mario = 0;


//********************************
// Declaración de constantes
//********************************
 
 // Pins de salida para los LED
  const int led_rojo = 5;  // 2 en arduino   // D1-5      
  const int led_azul = 4;  // 3 en arduino  // D2-4 
  const int led_amarillo = 2;  // 4 en arduino   // D4-2
  const int led_verde = 14;  // 5 en arduino  // D5-14
  
 // Pin para el zumbador piezoelectrico
  const int zumbador = 12;  // 6 en arduino  // D6-12
  const int tonePin = 12;
  
 // Pins de salida para los botones
  const int boton_rojo = 13;   // 8 en arduino   // D7-13
  const int boton_azul = 16;  // 9 en arduino  // D0-16 // ojo, el 15 no va
  const int boton_amarillo = 10;  // 10 en arduino  // SSD3-10
  const int boton_verde = 9;   // 11 en arduino  //  SSD2-9


//********************************
// Declaración de variables
//********************************

  long sequence[20];             // Array que alberga la secuencia
  int contador = 0;              // Contador
  long input = 5;                // Indicador de boton pulsado
  int wait = 500;                // Retraso segun la secuencia se incrementa
  int puntuacion_maxima = 3;    // Puntuación máxima donde acaba el juego 
 
 // Variables para los efectos musicales
  int length = 15;                  // Numero de notas de la melodia
  char notes[] = "ccggaagffeeddc "; // Notas de la melodia (cada letra es una nota distinta)
  int beats[] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 4 };  // Duracion de cada tono en un array
  int tempo = 100;  //Tempo de la melodia
 
 

//********************************
// Setup
//********************************
  void setup() {
   // Configuración de los pines de los leds y del zumbador como salidas
    pinMode(led_rojo, OUTPUT);      
    pinMode(led_verde, OUTPUT); 
    pinMode(led_amarillo, OUTPUT); 
    pinMode(led_azul, OUTPUT); 
    pinMode(zumbador, OUTPUT);
      
   // Configuración de los pines de los botones como entradas
    pinMode(boton_rojo, INPUT);    
    pinMode(boton_verde, INPUT);
    pinMode(boton_amarillo, INPUT);
    pinMode(boton_azul, INPUT);
      
    // Melodia de inicio al arrancar el Arduino    
    felicitacion();

    Serial.begin(115200);
  }
  
 
//********************************
// Programa principal
//********************************
  void loop() {
    mostrar_secuencia();  // Reproduce la sequencia
    leer_secuencia();     // Lee la sequencia
    delay(1000);          // Espera 1 segundo
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
      digitalWrite(zumbador, HIGH);
      delayMicroseconds(tone);
      digitalWrite(zumbador, LOW);
      delayMicroseconds(tone);
    }
  }
     
  
 
 // Funciones para encender los leds y reproducir el tono correspondiente
  void flash_rojo() {
    digitalWrite(led_rojo, HIGH);
    playtone(2273,wait);            
    digitalWrite(led_rojo, LOW);
  }
  
  void flash_azul() {
    digitalWrite(led_azul, HIGH);
    playtone(1700,wait);            
    digitalWrite(led_azul, LOW);
  }
  
  void flash_amarillo() {
    digitalWrite(led_amarillo, HIGH);
    playtone(1275,wait);             
    digitalWrite(led_amarillo, LOW);
  } 
  
  void flash_verde() {
    digitalWrite(led_verde, HIGH);
    playtone(1136,wait);             
    digitalWrite(led_verde, LOW);
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
    digitalWrite(led_rojo, HIGH);       
    digitalWrite(led_verde, HIGH);
    digitalWrite(led_amarillo, HIGH);
    digitalWrite(led_azul, HIGH);

    if (cancion_Mario == 1)
    {
      midi();
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
    digitalWrite(led_rojo, LOW);   
    digitalWrite(led_verde, LOW);
    digitalWrite(led_amarillo, LOW);
    digitalWrite(led_azul, LOW);
    resetcontador();

    if (cancion_Mario == 1)
    {
      fin_del_juego = 1;
    }
    
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
 
    // Con esto, la función Random es aun más aleatoria
    randomSeed(analogRead(8));

    // random(4) nos dará un valor aleatorio entre 0 y 3 (0,1,2,3)
    sequence[contador] = random(4); 
    // Imprimir por el Serial print la secuencia
    for (int i = 0; i < 20; i++) {  
      Serial.print(sequence[i]);             
    }
    delay(1000);
    Serial.println(" siguiente led");
           
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
        if (digitalRead(boton_rojo) == LOW) {    // LOW para Arduino
          input = 0;
            Serial.println(" boton_rojo_leido");
        }
        if (digitalRead(boton_verde) == LOW) {  
          input = 1;
        Serial.println(" boton_verde_leido");
        }
        if (digitalRead(boton_amarillo) == LOW) {
          input = 2;
        Serial.println(" boton_amarillo_leido");
        }
        if (digitalRead(boton_azul) == LOW) {   
          input = 3;
          Serial.println(" boton_azul_leido");
        }
      }// while
    
      if (sequence[i-1] == input) {             
        mostrar_boton_correcto(input);                           
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
          felicitacion();
          resetcontador();                          
      } 
      input = 5;                                   
    }// for
  } // void
