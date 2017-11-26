/** 
* @file utils.hpp
* @brief Generic utilities
* @author António Almeida
* @author João Borrego
*/

#pragma once

#include <math.h>
#include <Arduino.h>

#include "constants.hpp" 

namespace Utils {


    /**
     * @brief Converts the measured tension of an LDR to a LUX measurement
     *
     * r_ldr = r * ((v_max / v_in) - 1)
     * lux = 10 ^ ((log_10(r_ldr) - b) / a)
     *
     * @param v_in  The measured ldr tension (Volt)
     * @param a     Parameter of ldr characteristic
     * @param b     Parameter of ldr characteristic
     * @param v_max The maximum tension (Volt)
     * @param r     The value of resistance in series with the LDR (Ohm)
     *
     * @return The corresponding LUX value
     */
    float convertToLux(float v_in, float a, float b, float v_max=VCC, float r=LUX_R);

}
