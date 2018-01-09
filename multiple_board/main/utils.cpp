/** 
 * @file    multiple_board/main/utils.cpp
 * 
 * @brief   Generic utilities implementation
 * 
 * @author  João Borrego
 */

#include "utils.hpp"

namespace Utils {

    float convertToLux(float v_in, float a, float b, float v_max, float r){
          
        // Obtain resitance of LDR from tension divider
        float r_ldr = r * ((v_max / v_in) - 1);
        // Calculate LUX output
        float exp = (log10(r_ldr) - b)/a;
        float out = pow(10.0, exp);
        
        return out;
    }

    float average(float *array, int size){
        float avg = 0.0;
        for (int i = 0; i < size; i++){
            avg += array[i];
        }
        return avg / size;
    }
}
