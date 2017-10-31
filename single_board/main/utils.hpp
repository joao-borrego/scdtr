/** 
* @file utils.hpp
* @brief Generic utilities
* @author António Almeida
* @author João Borrego
*/

#include <math.h>

namespace Utils {

    /** Supply voltage in Volt */
    const float V_MAX = 5.0 ;
    /** Value of resistance in series with the ldr in Ohm */
    const float LUX_R = 10000.0;
    /** Parameter $a$ of the experimental characteristic of the ldr */    
    const float LUX_A = -0.62995;  // -0.75696;
    /** Parameter $b$ of the experimental characteristic of the ldr */    
    const float LUX_B = 4.8487;    // 4.2136;

    /**
     * @brief Converts the measured tension of an LDR to a LUX measurement
     *
     * r_ldr = r * ((v_max / v_in) - 1)
     * lux = 10 ^ ((log_10(r_ldr) - b) / a)
     *
     * @param v_in  The measured ldr tension (Volt)
     * @param v_max The maximum tension (Volt)
     * @param r     The value of resistance in series with the LDR (Ohm)
     * @param a     Parameter of ldr characteristic
     * @param b     Parameter of ldr characteristic
     *
     * @return The corresponding LUX value
     */
    float convertToLux(float v_in, float v_max=V_MAX, float r=LUX_R, float a=LUX_A, float b=LUX_B);
}
