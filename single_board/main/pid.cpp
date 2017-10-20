/** 
 * @file pid.cpp
 * @brief PID Controller
 * 
 * Proportional Integral Derivative Controller
 * 
 * @author António Almeida
 * @author João Borrego
*/

#include "pid.hpp"

namespace PID {

    inline Controller::Controller(
        float *input,
        float *output,
        float *reference,
        float k_p,
        float k_i,
        float k_d
    ){

        this->in = input;
        this->out = output;
        this->ref = reference;

        this->k_p = k_p;
        this->k_i = k_i;
        this->k_d = k_d;

        this->i = 0.0;
        this->d = 0.0;

        this->err = 0.0;
        this->y = 0.0;

        this->a = 0.0;
        this->b = 0.0;
        this->T = 0.0;

        this->k_1 = k_p * b;
        this->k_2 = k_p * k_i * T / 2.0;
        this->k_3 = k_d / (k_d + a * T);
        this->k_4 = (k_p * k_d * a) / (k_d + a * T);
    }

    void Controller::update(void (*updateFcn)(void)){

        /* Perform calculations */

        float y_new = *(in);
        float new_ref = *(ref);
        float err_new = new_ref - y_new;
        float p_new, i_new, d_new; 
        
        p_new = k_1 * new_ref - k_p * y;
        /* TODO - Apply threshold and saturate integrator term to avoid windup */
        i_new = i + k_2 * (err_new + err);
        d_new = k_3 * d - k_4 * (y_new - y);

        /* TODO - Apply deadzone to reduce output jitter */
        *(this->out) = p_new + i_new + d_new;        

        /* Update output using external callback function */
        updateFcn();

        /* Update old values */
        this->y = y_new;
        this->err = err_new;
        this->p = p_new;
        this->i = i_new;
        this->d = d_new;
    }

    /* Getters and Setters */

    float Controller::getK_p(){
        return this->k_p;
    }

    float Controller::getK_i(){
        return this->k_i;
    }

    float Controller::getK_d(){
        return this->k_d;
    }
}
