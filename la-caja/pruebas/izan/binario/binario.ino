
// Include Libraries
#include "Arduino.h"
#include "Button.h"


// Pin Definitions
#define PUSHBUTTON_PIN_2	5
#define TOGGLESWITCH_1_PIN_2	4
#define TOGGLESWITCH_2_PIN_2	0
#define TOGGLESWITCH_3_PIN_2	2
#define TOGGLESWITCH_4_PIN_2	14
#define TOGGLESWITCH_5_PIN_2	12
#define TOGGLESWITCH_6_PIN_2	13
#define TOGGLESWITCH_7_PIN_2	15
#define TOGGLESWITCH_8_PIN_2	16
#define TOGGLESWITCH_9_PIN_2	9



// Global variables and defines

// object initialization
Button pushButton(PUSHBUTTON_PIN_2);
Button ToggleSwitch_1(TOGGLESWITCH_1_PIN_2);
Button ToggleSwitch_2(TOGGLESWITCH_2_PIN_2);
Button ToggleSwitch_3(TOGGLESWITCH_3_PIN_2);
Button ToggleSwitch_4(TOGGLESWITCH_4_PIN_2);
Button ToggleSwitch_5(TOGGLESWITCH_5_PIN_2);
Button ToggleSwitch_6(TOGGLESWITCH_6_PIN_2);
Button ToggleSwitch_7(TOGGLESWITCH_7_PIN_2);
Button ToggleSwitch_8(TOGGLESWITCH_8_PIN_2);
Button ToggleSwitch_9(TOGGLESWITCH_9_PIN_2);


// define vars for testing menu
const int timeout = 10000;       //define timeout of 10 sec
char menuOption = 0;
long time0;

// Setup the essentials for your circuit to work. It runs first every time your circuit is powered with electricity.
void setup() 
{
    // Setup Serial which is useful for debugging
    // Use the Serial Monitor to view printed messages
    Serial.begin(9600);
    while (!Serial) ; // wait for serial port to connect. Needed for native USB
    Serial.println("start");
    
    pushButton.init();
    ToggleSwitch_1.init();
    ToggleSwitch_2.init();
    ToggleSwitch_3.init();
    ToggleSwitch_4.init();
    ToggleSwitch_5.init();
    ToggleSwitch_6.init();
    ToggleSwitch_7.init();
    ToggleSwitch_8.init();
    ToggleSwitch_9.init();
    menuOption = menu();
    
}

// Main logic of your circuit. It defines the interaction between the components you selected. After setup, it runs over and over again, in an eternal loop.
void loop() 
{
    
    
    if(menuOption == '1') {
    // Mini Pushbutton Switch - Test Code
    //Read pushbutton state. 
    //if button is pressed function will return HIGH (1). if not function will return LOW (0). 
    //for debounce funtionality try also pushButton.onPress(), .onRelease() and .onChange().
    //if debounce is not working properly try changing 'debounceDelay' variable in Button.h
    bool pushButtonVal = pushButton.read();
    Serial.print(F("Val: ")); Serial.println(pushButtonVal);

    }
    else if(menuOption == '2') {
    // ToggleSwitch #1 - Test Code
    //read Toggle Switch state. 
    //if Switch is open function will return LOW (0). 
    //if it is closed function will return HIGH (1).
    bool ToggleSwitch_1Val = ToggleSwitch_1.read();
    Serial.print(F("Val: ")); Serial.println(ToggleSwitch_1Val);
    }
    else if(menuOption == '3') {
    // ToggleSwitch #2 - Test Code
    //read Toggle Switch state. 
    //if Switch is open function will return LOW (0). 
    //if it is closed function will return HIGH (1).
    bool ToggleSwitch_2Val = ToggleSwitch_2.read();
    Serial.print(F("Val: ")); Serial.println(ToggleSwitch_2Val);
    }
    else if(menuOption == '4') {
    // ToggleSwitch #3 - Test Code
    //read Toggle Switch state. 
    //if Switch is open function will return LOW (0). 
    //if it is closed function will return HIGH (1).
    bool ToggleSwitch_3Val = ToggleSwitch_3.read();
    Serial.print(F("Val: ")); Serial.println(ToggleSwitch_3Val);
    }
    else if(menuOption == '5') {
    // ToggleSwitch #4 - Test Code
    //read Toggle Switch state. 
    //if Switch is open function will return LOW (0). 
    //if it is closed function will return HIGH (1).
    bool ToggleSwitch_4Val = ToggleSwitch_4.read();
    Serial.print(F("Val: ")); Serial.println(ToggleSwitch_4Val);
    }
    else if(menuOption == '6') {
    // ToggleSwitch #5 - Test Code
    //read Toggle Switch state. 
    //if Switch is open function will return LOW (0). 
    //if it is closed function will return HIGH (1).
    bool ToggleSwitch_5Val = ToggleSwitch_5.read();
    Serial.print(F("Val: ")); Serial.println(ToggleSwitch_5Val);
    }
    else if(menuOption == '7') {
    // ToggleSwitch #6 - Test Code
    //read Toggle Switch state. 
    //if Switch is open function will return LOW (0). 
    //if it is closed function will return HIGH (1).
    bool ToggleSwitch_6Val = ToggleSwitch_6.read();
    Serial.print(F("Val: ")); Serial.println(ToggleSwitch_6Val);
    }
    else if(menuOption == '8') {
    // ToggleSwitch #7 - Test Code
    //read Toggle Switch state. 
    //if Switch is open function will return LOW (0). 
    //if it is closed function will return HIGH (1).
    bool ToggleSwitch_7Val = ToggleSwitch_7.read();
    Serial.print(F("Val: ")); Serial.println(ToggleSwitch_7Val);
    }
    else if(menuOption == '9') {
    // ToggleSwitch #8 - Test Code
    //read Toggle Switch state. 
    //if Switch is open function will return LOW (0). 
    //if it is closed function will return HIGH (1).
    bool ToggleSwitch_8Val = ToggleSwitch_8.read();
    Serial.print(F("Val: ")); Serial.println(ToggleSwitch_8Val);
    }
    else if(menuOption == '10') {
    // ToggleSwitch #9 - Test Code
    //read Toggle Switch state. 
    //if Switch is open function will return LOW (0). 
    //if it is closed function will return HIGH (1).
    bool ToggleSwitch_9Val = ToggleSwitch_9.read();
    Serial.print(F("Val: ")); Serial.println(ToggleSwitch_9Val);
    }
    
    if (millis() - time0 > timeout)
    {
        menuOption = menu();
    }
    
}



// Menu function for selecting the components to be tested
// Follow serial monitor for instrcutions
char menu()
{

    Serial.println(F("\nWhich component would you like to test?"));
    Serial.println(F("(1) Mini Pushbutton Switch"));
    Serial.println(F("(2) ToggleSwitch #1"));
    Serial.println(F("(3) ToggleSwitch #2"));
    Serial.println(F("(4) ToggleSwitch #3"));
    Serial.println(F("(5) ToggleSwitch #4"));
    Serial.println(F("(6) ToggleSwitch #5"));
    Serial.println(F("(7) ToggleSwitch #6"));
    Serial.println(F("(8) ToggleSwitch #7"));
    Serial.println(F("(9) ToggleSwitch #8"));
    Serial.println(F("(10) ToggleSwitch #9"));
    Serial.println(F("(menu) send anything else or press on board reset button\n"));
    while (!Serial.available());

    // Read data from serial monitor if received
    while (Serial.available()) 
    {
        char c = Serial.read();
        if (isAlphaNumeric(c)) 
        {   
            
            if(c == '1') 
    			Serial.println(F("Now Testing Mini Pushbutton Switch"));
    		else if(c == '2') 
    			Serial.println(F("Now Testing ToggleSwitch #1"));
    		else if(c == '3') 
    			Serial.println(F("Now Testing ToggleSwitch #2"));
    		else if(c == '4') 
    			Serial.println(F("Now Testing ToggleSwitch #3"));
    		else if(c == '5') 
    			Serial.println(F("Now Testing ToggleSwitch #4"));
    		else if(c == '6') 
    			Serial.println(F("Now Testing ToggleSwitch #5"));
    		else if(c == '7') 
    			Serial.println(F("Now Testing ToggleSwitch #6"));
    		else if(c == '8') 
    			Serial.println(F("Now Testing ToggleSwitch #7"));
    		else if(c == '9') 
    			Serial.println(F("Now Testing ToggleSwitch #8"));
    		else if(c == '10') 
    			Serial.println(F("Now Testing ToggleSwitch #9"));
            else
            {
                Serial.println(F("illegal input!"));
                return 0;
            }
            time0 = millis();
            return c;
        }
    }
}

/*******************************************************

*    Circuito.io is an automatic generator of schematics and code for off
*    the shelf hardware combinations.

*    Copyright (C) 2016 Roboplan Technologies Ltd.

*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.

*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.

*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*    In addition, and without limitation, to the disclaimers of warranties 
*    stated above and in the GNU General Public License version 3 (or any 
*    later version), Roboplan Technologies Ltd. ("Roboplan") offers this 
*    program subject to the following warranty disclaimers and by using 
*    this program you acknowledge and agree to the following:
*    THIS PROGRAM IS PROVIDED ON AN "AS IS" AND "AS AVAILABLE" BASIS, AND 
*    WITHOUT WARRANTIES OF ANY KIND EITHER EXPRESS OR IMPLIED.  ROBOPLAN 
*    HEREBY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT 
*    NOT LIMITED TO IMPLIED WARRANTIES OF MERCHANTABILITY, TITLE, FITNESS 
*    FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT, AND THOSE ARISING BY 
*    STATUTE OR FROM A COURSE OF DEALING OR USAGE OF TRADE. 
*    YOUR RELIANCE ON, OR USE OF THIS PROGRAM IS AT YOUR SOLE RISK.
*    ROBOPLAN DOES NOT GUARANTEE THAT THE PROGRAM WILL BE FREE OF, OR NOT 
*    SUSCEPTIBLE TO, BUGS, SECURITY BREACHES, OR VIRUSES. ROBOPLAN DOES 
*    NOT WARRANT THAT YOUR USE OF THE PROGRAM, INCLUDING PURSUANT TO 
*    SCHEMATICS, INSTRUCTIONS OR RECOMMENDATIONS OF ROBOPLAN, WILL BE SAFE 
*    FOR PERSONAL USE OR FOR PRODUCTION OR COMMERCIAL USE, WILL NOT 
*    VIOLATE ANY THIRD PARTY RIGHTS, WILL PROVIDE THE INTENDED OR DESIRED
*    RESULTS, OR OPERATE AS YOU INTENDED OR AS MAY BE INDICATED BY ROBOPLAN. 
*    YOU HEREBY WAIVE, AGREE NOT TO ASSERT AGAINST, AND RELEASE ROBOPLAN, 
*    ITS LICENSORS AND AFFILIATES FROM, ANY CLAIMS IN CONNECTION WITH ANY OF 
*    THE ABOVE. 
********************************************************/