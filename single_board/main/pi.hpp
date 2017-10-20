/** 
 * @file pi.hpp
 * @brief PI Controller headers
 * 
 * Proportional Integral Controller
 * 
 * @author António Almeida
 * @author João Borrego
*/

namespace PI {

    class Controller {

    private:

        /* @brief Pointer to input var */
        float *in;
        /* @brief Pointer to output var */
        float *out;
        /* @brief Pointer to reference var */
        float *ref;

        /* Coefficients */

        /* @brief Coefficient for proportional term */
        float k_p;
        /* @brief Coefficient for integral term */
        float k_i;

        /* @brief Proportional term */
        float p;
        /* @brief Integral term */
        float i;

        /* @brief Input of the controller */
        float y;
        /* @brief Error with respect to the reference */
        float err;

        /* @brief Constant between 0 and 1 */
        float b;
        /* @brief Sampling time */
        float T;

        /* Numerical constants */

        // p = k_p * b - k_p * y
        /* @brief k_1 = k_p * b */
        float k_1;

        // i = i_old + (k_p * k_i * T / 2) * (err - err_old)
        /* @brief k_2 = k_p * k_i * T / 2 */
        float k_2;

        /* @brief Pointer to callback function to update output */
        void *(updateFcn)();

    private:

        /* DEBUG Number of steps calculated */
        int steps = 0;

    public:

        /**
         * @brief      Constructor
         *
         * @param      input      The pointer to the input var
         * @param      output     The pointer to the output var
         * @param      reference  The pointer to the reference var
         * @param      k_p        The proportional term coefficient
         * @param      k_i        The integral term coefficient
         * @param      T          The sampling time
         */
        inline Controller(
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
         * @param	   updateFcn	The pointer to the output callback fuction
         */
        void update(void (*updateFcn)(void));
    };
}
