/** 
 * @file main.ino
 * @brief Multiple board system main application
 * @author António Almeida
 * @author João Borrego
 */

#include <EEPROM.h>
#include <Wire.h>

#include "constants.hpp"
#include "utils.hpp"

/* Pinout */

/** LDR analog in pin */
const int pin_ldr = A0;
/** LED PWM out pin */
const int pin_led = 11;

/* Global variables */

/** Device id */
uint8_t id{0};

/** LDR input ADC value */
volatile float ldr_in {0};
/** LDR input in Volt */
volatile float v_in {0};
/** LDR input converted to LUX units */
volatile float input {0};

/** Latest elapsed milliseconds since startup */
unsigned long current_millis {0};
/** Previously recorded elapsed milliseconds since startup */
unsigned long last_millis {0};

// TEMP
char recv;

void onReceive(){
    recv = Wire.read();
    Serial.println(recv);
}


/**
 * @brief      Arduino setup
 */
void setup() {
    /* Read device ID from EEPROM */
    id = EEPROM.read(ID_ADDR);
    /* Setup serial communication */
    Serial.begin(BAUDRATE);
    /* Setup I2C communication */
    if (id == 0){
        // Join the bus as master
        Wire.begin();
    } else {
        Wire.begin(id);
        Wire.onReceive(onReceive);
    }
}

/**
 * @brief      Arduino loop
 */
void loop() {
    
    /* Print status every STATUS_DELAY milliseconds */
    current_millis = millis();
    if (current_millis - last_millis >=  STATUS_DELAY){
        last_millis = current_millis;

        if (id == 0){
            Wire.beginTransmission(1);
            Wire.write('A');
            Wire.endTransmission();
        }

        /* Optional: lower CPU usage */
        delay(0.7 * STATUS_DELAY);
    }
}

/**
 * @brief      Lists important variables to Serial
 */
void listVariables(){

    ldr_in = analogRead(pin_ldr);
    v_in = ldr_in * (VCC / 1023.0);
    input = Utils::convertToLux(v_in, LUX_A[id], LUX_B[id]);

    Serial.print(id, DEC);
    Serial.print("\t");
    Serial.print(ldr_in);
    Serial.print("\t");
    Serial.println(input);
}