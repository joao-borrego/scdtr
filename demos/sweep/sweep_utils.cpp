/** 
* @file sweep_utils.cpp
* @brief Generic utilities
* @author António Almeida
* @author João Borrego
*/

#include "sweep_utils.hpp"

namespace Utils {

  float convertToLux(float v_in, float v_max, float r, float a, float b){
        
    /* Obtain resitance of LDR from tension divider */
    float r_ldr = r * ((v_max / v_in) - 1);
    /* Calculate LUX output */
    float exp = (log10(r_ldr) - b)/a;
    float out = pow(10.0, exp);
    
    return out;
  }
}
