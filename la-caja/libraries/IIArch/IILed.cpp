#include "IILed.h"


void IILed::Setup(int pinled) { 
    pinled_     = pinled;
    if (pinled_ > 0) {
        pinMode(pinled, OUTPUT); 
    }
    lighted_ = false;
}

bool IILed::HasChanged(void){
    static unsigned int lastchange = millis();
    unsigned int now = millis();
    static bool newState = false;
    float maxLevel = 0.5;
    static int iDigital =0;

    if (now > lastchange + 500) {
        Serial.printf("%c", newState?'1':'0');
        lastchange = now;
        newState = !newState;
        maxLevel = 0.5;
        return true;
    }
    maxLevel -= 0.001;   
    if (iDigital++ % 20 < 10) {
        analogWrite(pinled_, PctToLight(maxlevel_));    
    }
    else {
        digitalWrite(pinled_, newState?LOW:HIGH);
    }
    
    return false;
}

int IILed::PctToLight(float percent) {
    return 255-(int)(percent*255.0);
}


void IILed::SetLight(bool lighted, float maxlevel) { 
    lighted_ = lighted;
    maxlevel_ = maxlevel;

    if (pinled_ < 0) { // no hacer nada si no hay pin conectado
        ;
    }
    else if (!lighted && lighted_) { // apagar
        digitalWrite(pinled_, HIGH);
    }
    else if (lighted && !lighted_ && maxlevel_ == 1.0) { // encender
        digitalWrite(pinled_, LOW);
    }
    else if (lighted && !lighted_ && maxlevel_ < 1.0) { // modo luminaria
        analogWrite(pinled_, PctToLight(maxlevel_));
    }
    // else el led no cambia de estado
}



