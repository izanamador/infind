#include "EspInfInd.hpp"
/*
  Librerías requeridas
    - DHT sensor library for ESPx
    - PubSub-Cient by Nick O'Leary
*/
/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
*/

#include "DHTesp.h" // Click here to get the library: http://librarymanager/All#DHTesp
#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif
DHTesp dht;

#define PIN_DHT11 2 // SENSOR CONECTADO A D2=GPIO4-SDA

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_DHT11, INPUT);
  
  Serial.begin(115200);

  Serial.println();
  Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)\tHeatIndex (C)\t(F)");
  String thisBoard= ARDUINO_BOARD;
  Serial.println(thisBoard);

  // Autodetect is not working reliable, don't use the following line
  // dht.setup(17);
  // use this instead: 
  dht.setup(PIN_DHT11, DHTesp::DHT11); // Connect DHT sensor to GPIO2 DHT22->DHT11

}

// the loop function runs over and over again forever
void loop() {
  
  const unsigned long msInterval = 5000; // five seconds
  const unsigned long msSampling = dht.getMinimumSamplingPeriod();
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
      float humidity = dht.getHumidity();
      float temperature = dht.getTemperature();
      sprintf(strMessage, "Status=%s, Hum=%0.2f, Temp=%0.1f ºC=%0.1f ºF, Heat=", 
        dht.getStatusString(), humidity, temperature, dht.toFahrenheit(temperature));
      Serial.print(strMessage);
      Serial.print(dht.computeHeatIndex(temperature, humidity, false), 1);
      Serial.println("ºC");
      //Serial.println(dht.computeHeatIndex(dht.toFahrenheit(temperature), humidity, true), 1);//char strMessage[80];
    }
  }
}


