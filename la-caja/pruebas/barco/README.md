
<div align="center">

  <img src="https://www.3dbenchy.com/wp-content/uploads/2016/01/cropped-3DBenchy_website_logo_512x512px.png" alt="logo" width="200" height="auto" />
  <h1>Prueba 5: El barco</h1>
  
  <p>
    Enviar SOS en código Morse mediante linterna a LDR 
  </p>
  
   
<h4>
    <a href="https://www.youtube.com/watch?v=k8m8R4x0Mgk">View Solution Demo</a>
  <span> · </span>
    <a href="https://github.com/izanamador/infind">Documentation</a>
  <span> · </span>
    <a href="https://github.com/izanamador/infind/issues/">Report Bug</a>
  <span> · </span>
    <a href="https://github.com/izanamador/infind/issues/">Request Feature</a>
  </h4>
</div>

<br />

<!-- Table of Contents -->
# Tabla de Contenidos

- [Descripción de la prueba](#descripcion)
  * [Información Dashboard](#dart-features)
  * [Información Bot de Telegram](#art-color-reference)
    #### [Pistas](#art-color-reference)
- [Hardware necesario](#ampliaciones)
  * [Esquemático técnico](#opcion1)
  * [Esquemático usuario](#opcion1)
- [Pseudocódigo](#ampliaciones)
  * [C++](#opcion1)
  * [NodeRED](#opcion1)
  #### [Base de datos](#art-color-reference)
- [Ampliaciones](#ampliaciones)
  * [Opción 1](#opcion1)
  * [Opción 2](#opcion1)


<!-- About the Project -->
# Descripción de la prueba 5

Se dispone en una de las caras del cubo de un LDR escondido en un barco impreso en 3D o en una imagen. Al usuario se le proporciona una descripción sobre la necesidad de ser rescatado de una isla desierta. Para solucionar la prueba, se debe de introducir la secuencia la traducción del mensaje S.O.S en código MORSE generando la señal de luz mediante la linterna del móvil. 


## Información Dashboard

- Opción 1: Se mostrará una imagen de una isla desierta con algún tipo de foco.

- Opción 2: Se mostrará una imagen del puerto de Málaga o de similares con un las luces correspondientes. 

La idea es generar dos imágenes distintas. Una con las luces encendidas y otra con las luces apagadas. De esta manera, cuando el usuario llegue al umbral definido alumbrando con la linterna al LDR en el dashboard se actualizará en tiempo real la imagen para que sea consciente de que está mandando una señal positiva. 

## Información Bot de Telegram

"Llevaís en una isla desierta varios días sin comida ni agua, y divisáis un barco a lo lejos... Es el momento de aprovechar vuestra oportunidad para salvaros."

- Opción 1: Enunciado corto pero estándart.


- Opción 2: Enunciado Malaguita.

- Opción 3: Enunciado extenso con detalles innecesarios. 

 ### Pistas
 1.  Idea de usar la linterna
 2.  Idea de usar código morse o link de herramienta de conversor a morse
 3.  Idea de mandar S.O.S 
 

# Hardware Necesario

El hardware de la prueba consistirá únicamente en un sensor LDR conectado a la ESP8266. 

https://datasheetspdf.com/pdf-file/1402029/Joy-IT/KY-018/1
https://www.nteinc.com/resistor_web/pdf/LDR-Series.pdf

Pines:

- Data (Analog)
- Vcc (5 V)
- Gnd

Se trata de un sensor analógico por lo que habrá que conectarlo a la ADC0 de la ESP8266. 

El integrado presenta un divisor de tensión por lo que habrá que acondicionar la señal adecuadamente en el .ino si se pretenden leer valores intermedios. 


## [Esquemático Técnico](https://shields.io/)
## [Esquemático Usuario](https://shields.io/)

# Pseudocógido

https://create.arduino.cc/projecthub/shjin/adaptive-led-morse-code-decoder-and-timer-interrupt-8d18a7

## C++

``` C++
umbral_luz = valor max; / Saturar sensor

umbral_punto = umbral_t1;
umbral_guion = umbral_t2;

lectura ldr ()
  if valor_lectura == umbral_luz during umbral_punto then
  msg = "."
  else if valor_lectura == umbral_luz during umbral_guion then
  msg = "-"
  else
  msg = " "

```
## NodeRED

```NodeRED
begin:
mqtt
  json node
    change svg or image
    mongo db 
    correct answer checker

```



# Ampliaciones

## Opción 1

Módulo adherido a la caja emisor de luz mediante led, cable y accionado mediante botón para ser autocontenido. 

## Opción 2

Bot de telegram o dashboard con lectura del mensaje en morse en tiempo real ó implementar herramienta de codificador de morse para ayudar al usuario.  