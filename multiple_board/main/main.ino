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
#include "calibration.hpp"

/* Global variables */

/** Device id */
uint8_t id{0};

/** K parameter matrix */
float k[N*N]{0};

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

/**
 * @brief      Arduino setup
 */
void setup() {
    /* Read device ID from EEPROM */
    id = EEPROM.read(ID_ADDR);
    /* Setup serial communication */
    Serial.begin(BAUDRATE);
    /* Setup I2C communication */
    if (id == MASTER){
        Wire.begin();
        Serial.print("[I2C] Registered as Master - ");
    } else {
        Wire.begin(id);
        Wire.onReceive(Calibration::onReceive);
        Wire.onRequest(Calibration::onRequest);
        Serial.print("[I2C] Registered as Slave - ");
    }
    Serial.println(id);

    /* Determine K matrix */
    Calibration::execute(k, N, id);
}

/**
 * @brief      Arduino loop
 */
void loop() {
    
    // TODO
}