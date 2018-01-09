/**
 * @file    multiple_board/main/calibration.hpp
 * 
 * @brief   Calibration utility headers
 * 
 * Provides a calibration utility meant to run at system startup
 * Calculates external illuminance and K_i vector
 * 
 * @author  João Borrego
 */

#ifndef INO_CALIBRATION_HPP
#define INO_CALIBRATION_HPP

#include <Arduino.h>
#include <Wire.h>

#include "utils.hpp"
#include "constants.hpp"
#include "communication.hpp"

namespace Calibration{
    
    /** Minimum wait between I2C messages */
    #define WAIT        10
    /** Minimum wait between LED output value changes */
    #define WAIT_LED    90

    /** Maximum I2C packet size (bytes) */
    #define MAX_PACKET_SIZE 32

    /** Sample values of duty-cycle for which to obtain illuminance */
    #define SAMPLES {\
        15.0,  25.0,  35.0,  45.0,  55.0 ,\
        65.0,  75.0,  85.0,  95.0,  105.0,\
        115.0, 125.0, 135.0, 145.0, 155.0,\
        165.0, 175.0, 185.0, 195.0, 205.0,\
        215.0, 225.0, 235.0, 245.0, 255.0 \
    }
    /** Number of samples */
    #define NUM_SAMPLES 25

    /** Obtain external illuminance */
    #define STATE_EXT   0
    /** Obtain illuminance value */
    #define STATE_ON    1
    /** Wait */
    #define STATE_OFF   2

    /**
     * @brief      Callback function for receiving I2C messages.
     *
     * @param[in]  bytes  The number of bytes available
     */
    void onReceive(int bytes);

    /**
     * @brief      Executes calibration.
     *
     * @param[in]  id_   The node identifier
     * @param      k_i   The K_i vector
     * @param      o_i   The external illuminance
     */
    void execute(int id_, float *k_i, float *o_i);

    /**
     * @brief      Gets the ldr value in lux.
     *
     * @return     The ldr value (lux).
     */
    float getLDRValue();
}

#endif
