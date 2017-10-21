/** 
* @file utils.hpp
* @brief Generic utilities
* @author António Almeida
* @author João Borrego
*/

/** Supply voltage in Volt */
#define V_MAX (float) 5.0 
/** Value of resistance in series with the ldr in Ohm */
#define LUX_R (float) 10000.0
/** Parameter $a$ of the experimental characteristic of the ldr */    
#define LUX_A (float) -0.62995  // -0.75696
/** Parameter $b$ of the experimental characteristic of the ldr */    
#define LUX_B (float) 4.8487    // 4.2136

#include <math.h>

namespace Utils {

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
