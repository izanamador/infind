#include <Keypad.h>

const byte n_rows = 4;
const byte n_cols = 4;

char keys[n_rows][n_cols] = {
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};

byte rowPins[n_rows] = {D0, D1, D2, D3};
byte colPins[n_cols] = {D4, D5, D6, D7};

Keypad myKeypad = Keypad( makeKeymap(keys), rowPins, colPins, n_rows, n_cols); 

void setup(){
Serial.begin(115200);
}

void loop(){
char myKey = myKeypad.getKey();
if (myKey != NULL){
Serial.print("Key pressed: ");
Serial.println(myKey);
}
}