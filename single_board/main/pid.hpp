/** 
 * @file pid.hpp
 * @brief PID Controller headers
 * 
 * Proportional Integral Derivative Controller
 * 
 * @author António Almeida
 * @author João Borrego
*/

namespace PID {

    class Controller {

    private:

        /* @brief Input */
        float *in;
        /* @brief Output */
        float *out;
        /* @brief Reference */
        float *ref;

        /* Coefficients */

        /* @brief Coefficient for proportional term */
        float k_p;
        /* @brief Coefficient for integral term */
        float k_i;
        /* @brief Coefficient for derivative term */
        float k_d;

        /* @brief ??? */
        float a;
        /* @brief ??? */
        float b;
        /* @brief Sample time */
        float T;

        /* Numerical constants */

        // p = k_p * b - k_p * y
        /* @brief k_1 = k_p * b */
        float k_1;

        // i = i_old + (k_p * k_i * T / 2) * (err - err_old)
        /* @brief k_2 = k_p * k_i * T / 2 */
        float k_2;

        // d = d_old * (k_d / (k_d + a / T)) -
        //  - ((k_p * K_d * a)/(k_d + a * T)) * (y - y_old)  
        /* @brief k_3 = k_d / (k_d + a / T) */
        float k_3;
        /* @brief k_4 = (k_p * k_d * a)/(k_d + a * T) */
        float k_4;

        /* @brief Pointer to callback function to update output */
        void *(updateFcn)();

    private:

        /* DEBUG Number of steps calculated */
        int steps = 0;

    public:

        /**
         * @brief      Constructs the object.
         *
         * @param      input       The input ptr
         * @param      output      The output ptr
         * @param      reference   The reference ptr
         * @param      k_p         The k_p value
         * @param      k_i         The k_i value
         * @param      k_d         The k_d value
         * @param      update_fcn  The update output function ptr
         */
        inline Controller(
            float *input,
            float *output,
            float *reference,
            float k_p,
            float k_i,
            float k_d,
            void (*update_fcn)(void)
        );    

        /**
         * @brief      Updates output
         */
        void update();

        /* Getters and Setters */
        
        /**
         * @brief      Gets the coefficient for the proportional term.
         * @return     k_p.
         */
        float getK_p();

        /**
         * @brief      Gets the coefficient for the integral term.
         * @return     k_i.
         */
        float getK_i();

        /**
         * @brief      Gets the coefficient for the derivative term.
         * @return     k_d.
         */
        float getK_d();

    };
}
