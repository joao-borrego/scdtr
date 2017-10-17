/** 
* @main.ino
* @brief Generic utilities
* @author António Almeida
* @author João Borrego
*/

#include "utils.hpp"

float lux_value = 0.0;
float ldr_in = 0.0; 
float v_in = 0.0;
float lookup[256] = LOOKUP_TABLE;

int pin_ldr = A0;

void setup() {
    /* */
    Serial.begin(9600);
    
}

void loop() {

    for (int out = 0; out <= 255; out++){
      analogWrite(11, out);
      delay(50);
      ldr_in =  analogRead(A0);
      v_in = ldr_in * (5.0 / 1023.0);
      lux_value = Utils::convertToLux(v_in);
      Serial.println(lux_value);
    }
}
