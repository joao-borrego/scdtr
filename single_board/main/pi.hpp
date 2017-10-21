/** 
 * @file pi.hpp
 * @brief PI Controller headers
 * 
 * Proportional Integral Controller
 * 
 * @author António Almeida
 * @author João Borrego
*/

#include "Arduino.h"

namespace PIController {

    class Controller {

    private:

        /** Pointer to input var */
        float *in;
        /** Pointer to output var */
        float *out;
        /** Pointer to reference var */
        float *ref;

        /* Coefficients */

        /** Coefficient for proportional term */
        float k_p;
        /** Coefficient for integral term */
        float k_i;

        /** Proportional term */
        float p;
        /** Integral term */
        float i;

        /** Input of the controller */
        float y;
        /** Error with respect to the reference */
        float err;

        /** Constant between 0 and 1 */
        float b;
        /** Sampling time */
        float T;

        /* Numerical constants */

        /** Error deadzone threshold */
        float err_deadzone = 3.0;

        /**
         * k_1 = k_p * b
         * p = k_p * b - k_p * y
         */
        float k_1;

        /**
         * k_2 = k_p * k_i * T / 2
         * i = i_old + (k_p * k_i * T / 2) * (err - err_old)
         */
        float k_2;

    public:

        /**
         * @brief      Constructor
         *
         * @param      input      The pointer to the input var
         * @param      output     The pointer to the output var
         * @param      reference  The pointer to the reference var
         * @param      k_p        The proportional term coefficient
         * @param      k_i        The integral term coefficient
         * @param      T          The sampling time in seconds
         */
        Controller(
            float *input,
            float *output,
            float *reference,
            float k_p,
            float k_i,
            float T
        );    

        /**
         * @brief      Updates output
         *
         * @param      updateFcn    The pointer to the output callback fuction
         */
        void update(void (*updateFcn)(void));
    };
}
