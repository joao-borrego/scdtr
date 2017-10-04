/** 
* @file utils.hpp
* @brief Generic utilities
* @author António Almeida
* @author João Borrego
*/

#define V_MAX 5.0 // Volt
#define LUX_R 100.0 // Ohm      
#define LUX_A 1.0
#define LUX_B 1.0

#include <math.h>

namespace Utils {

    /**
     * @brief      { function_description }
     *
     * r_ldr = r * ((v_max / v_in) - 1)
     * lux = 10 ^ ((log_10(r_ldr) - b) / a)
     *
     * @param[in]  v_in   The v in
     * @param[in]  v_max  The v maximum
     * @param[in]  r      { parameter_description }
     * @param[in]  a      { parameter_description }
     * @param[in]  b      { parameter_description }
     *
     * @return     { description_of_the_return_value }
     */
    float convertToLux(float v_in, float v_max=V_MAX, float r=LUX_R, float a=LUX_A, float b = LUX_B);
}