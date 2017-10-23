/**
 * @file pi.cpp
 * @brief PI Controller
 *
 * Proportional Integral Controller
 *
 * @author António Almeida
 * @author João Borrego
*/

#include "pi.hpp"

namespace PIController {

    Controller::Controller(
        float *input,
        float *output,
        float *reference,
        float k_p,
        float k_i,
        float T
    ){

        this->in = input;
        this->out = output;
        this->ref = reference;

        this->k_p = k_p;
        this->k_i = k_i;

        this->i = 0.0;

        this->err = 0.1;
        this->y = 0.0;

        this->b = 6.5;
        this->T = T;

        this->k_1 = k_p * b;
        this->k_2 = k_p * k_i * T / 2.0;
    }

    void Controller::update(void (*updateFcn)(void)){

        /* Perform calculations */

        float y_new = *(in);
        float new_ref = *(ref);

        float err_new = new_ref - y_new;
        
        /* Apply deadzone to reduce output jitter */
        float err_diff = err_new - err;
        if (abs(err_diff) < err_deadzone){
            err_new = 0.0;
        }

        float p_new, i_new;

        p_new = k_1 * new_ref - k_p * y;

        /* TODO - Apply threshold and saturate integrator term to avoid windup */
        i_new = i + k_2 * (err_new + err);

        *(this->out) = p_new + i_new;

        /* Update output using external callback function */
        updateFcn();

        /* Update old values */
        this->y = y_new;
        this->err = err_new;
        this->p = p_new;
        this->i = i_new;
    }
}
