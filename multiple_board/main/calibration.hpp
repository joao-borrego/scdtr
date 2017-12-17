/** 
* @file calibration.hpp
* @brief Calibration utility headers
* @author João Borrego
* @author António Almeida
*/

#pragma once

#include <Arduino.h>
#include <Wire.h>

#include "utils.hpp"
#include "constants.hpp"
#include "communication.hpp"

/** Minimum wait between I2C messages */
#define WAIT        20
/** Minimum wait between LED output value changes */
#define WAIT_LED    150

/** Maximum I2C packet size (bytes) */
#define MAX_PACKET_SIZE 32

/** Sample values of duty-cycle for which to obtain lux*/
#define SAMPLES {\
    111.0, 127.0, 143.0, 159.0, 175.0,\
    191.0, 207.0, 223.0, 239.0, 255.0 \
}
/** Number of samples */
#define NUM_SAMPLES 10

/** Obtain external illuminance */
#define STATE_EXT   0
/** Obtain illuminance value */
#define STATE_ON    1
/** Wait */
#define STATE_OFF   2

namespace Calibration{

    /**
     * @brief      Callback function for receiving I2C messages.
     *
     * @param[in]  bytes  The bytes
     */
    void onReceive(int bytes);

    /**
     * @brief      Executes calibration.
     *
     * @param[in]  id_   The node identifier
     * @param      k_i   The K_i sub-matrix
     * @param      o_i   The external lux
     */
    void execute(int id_, float *k_i, float *o_i);

    /**
     * @brief      Gets the ldr value in lux.
     *
     * @return     The ldr value (lux).
     */
    float getLDRValue();
}
