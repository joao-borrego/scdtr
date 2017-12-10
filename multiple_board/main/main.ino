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
#include "communication.hpp"
#include "calibration.hpp"
#include "consensus.hpp"

/* Global variables */

/** Device id */
uint8_t id{0};

/** Row i of K parameter matrix */
float k_i[N]{0};
/** LUX value for complete darkness */
float o_i{0};

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
    
    // Setup serial communication
    Serial.begin(BAUDRATE);
    
    //Read device ID from EEPROM
    id = EEPROM.read(ID_ADDR);
    Serial.print("[I2C] Registered with id ");
    Serial.println((int) id);
    

    // Begin I2C communication 
    Communication::setDeviceId(id);
    Wire.begin(id);
    /*
    // Determine K matrix and external illuminance
    Wire.onReceive(Calibration::onReceive);
    Wire.onRequest(Calibration::onRequest);
    Calibration::execute(k_i, &o_i, id);

    // Setup I2C communication for main loop 
    Wire.onReceive(Communication::onReceive);
    */
}

/**
 * @brief      Arduino loop
 */
void loop() {

    // Packets have to be acknowledged in order to be sniffed:
    // Each device acks a single packet
    Communication::sendInfo((id + 1) % N, input, 0.0, o_i, false);

    // if start consensus
    
    Wire.onReceive(Communication::nop);

    float k_i_tmp[N][N] = {{2.0, 1.0}, {1.0, 2.0}};
    int d_best;
    if (id == 0){
        d_best = Consensus::solve(id, 150.0, k_i_tmp[0], 30.0);
    }else{
        d_best = Consensus::solve(id, 80.0,  k_i_tmp[1],  0.0);
    }
    Serial.println(d_best);

    delay(100000);
}