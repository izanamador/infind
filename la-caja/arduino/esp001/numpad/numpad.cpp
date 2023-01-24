// librerías generales
#include <array>
#include <string.h>
#include <Wire.h>
#include <math.h>
#include <cstring>

#include <Keypad.h> // Libreria para usar el numpad 


//---- Creación del objeto myKeypad
#define NUM_ROWS 4  
#define NUM_COLS 4
char keys[NUM_ROWS][NUM_COLS] = {
  /* Layout */
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[NUM_ROWS] = {D0, D1, D2, D3}; 
byte colPins[NUM_COLS] = {D4, D5, D6, D7}; 
Keypad myKeypad = Keypad(makeKeymap(keys), rowPins, colPins, NUM_ROWS, NUM_COLS); 


#define PARENT_CONSTRUCTOR(DERIVED, PARENT)                    \
template<typename... Args>                                     \
DERIVED(Args&&... args) : PARENT(std::forward<Args>(args)...)

class Derived : public Parent
{
public:
  PARENT_CONSTRUCTOR(Derived, Parent)
  {
  }
};
    
class JuegoNumpad: public JuegoInfInd
{
public:
    PARENT_CONSTRUCTOR(Derived, Parent) { }
    void Loop();
private:
    int iDigit_ = 0;
    char lastKey_ = '\0';
};

void JuegoNumpad::Loop() {
    const int maxDigits = 10;
    // si se pulsó un atecla numérica acumularla en el string
        if ((lastKey_ >='0') && (lastKey_ <='9') && (iDigit_ <= maxDigits)) {
            gamedata[iDigit_++] = lastKey_;
            gamedata[iDigit_] = '\0';
            ReportStatus(gamedata); // refrescar la pantalla
        }
    // si se pulsó # (enviar), reportar si se ganó o se perdió y preparar la siguiente vida
        else if (lastKey_ == '#') {    
            if (strcmp(gamedata, gameparam)==0) {
                ReportSuccess(gamedata);
            } 
            else {
                ReportFail(gamedata);
            }
            iDigit_ = 0; 
            gamedata[iDigit_] = '\0';
        }
}