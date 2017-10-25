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

#define ANTI_WINDUP_SAT_MIN -1.0
#define ANTI_WINDUP_SAT_MAX  1.0
#define ERROR_DEADZONE       0.0

namespace PIController {

    class Controller {

    private:

        /** Pointer to input var */
        volatile float *in;
        /** Pointer to output var */
        volatile float *out;
        /** Pointer to reference var */
        volatile float *ref;

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
        float err_deadzone = ERROR_DEADZONE;
        /** Anti Windup negative saturation threshold */
        float sat_min = ANTI_WINDUP_SAT_MIN;
        /** Anti Windup positive saturation threshold */
        float sat_max = ANTI_WINDUP_SAT_MAX;

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
            volatile float *input,
            volatile float *output,
            volatile float *reference,
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

        /**
         * @brief      Updates internal coefficients
         *
         * @param      k_p   The k p
         * @param      k_i   The k i
         */
        void updateCoefficients(float k_p, float k_i);

        /**
         * @brief      Sets the error deadzone.
         *
         * @param      deadzone  The deadzone
         */
        void setErrorDeadzone(float deadzone);

        /**
         * @brief      Sets the anti windup saturation parameters.
         *
         * @param      sat_min  The saturation minimum
         * @param      sat_max  The saturation maximum
         */
        void setAntiWindupSat(float sat_min, float sat_max);
    };
}
