/** 
* @main.ino
* @brief Generic utilities
* @author António Almeida
* @author João Borrego
*/

#include "utils.hpp"

float r_ldr = 0.0;
float ldr_in = 0.0; 
float v_in = 0.0;

int pin_ldr = A0;

void setup() {
    /* */
    Serial.begin(9600);
    
}

void loop() {

    delay(100);
    ldr_in =  analogRead(A0);
    v_in = ldr_in * (5.0 / 1023.0);
    r_ldr = Utils::convertToLux(v_in);
    Serial.println(r_ldr);  
}
