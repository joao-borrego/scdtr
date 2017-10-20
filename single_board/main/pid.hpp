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
        /* @brief Coefficient for derivative term */
        float k_d;

         /* @brief Proportional term */
        float p;
         /* @brief Integral term */
        float i;
         /* @brief Derivative term */
        float d;

        /* @brief Input of the controller */
        float y;
        /* @brief Error with respect to the reference */
        float err;

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
         * @brief      Constructor
         *
         * @param      input      The pointer to the input var
         * @param      output     The pointer to the output var
         * @param      reference  The pointer to the reference var
         * @param      k_p        The proportional term coefficient
         * @param      k_i        The integral term coefficient
         * @param      k_d        The derivative term coefficient
         */
        inline Controller(
            float *input,
            float *output,
            float *reference,
            float k_p,
            float k_i,
            float k_d
        );    

        /**
         * @brief      Updates output
         *
         * @param	   updateFcn	The pointer to the output callback fuction
         */
        void update(void (*updateFcn)(void));

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
