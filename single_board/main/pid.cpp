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
        float k_d,
        void (*update_fcn)(void)
    ){

        this->in = input;
        this->out = output;
        this->ref = reference;

        this->k_p = k_p;
        this->k_i = k_i;
        this->k_d = k_d;

        this->a = 0.0;
        this->b = 0.0;
        this->T = 0.0;

        this->k_1 = k_p * this->b;
        this->k_2 = k_p * k_i * this->T / 2.0;
        this->k_3 = k_d / (k_d + this->a / this->T);
        this->k_4 = (k_p * k_d * this->a) / (k_d + this->a * this->T);

        //this->updateFcn = update_fcn;
    }

    void Controller::update(){

        /* Perform calculations */

        float err = ref - this->in;

        /* Update output using external callback function */
        //this->updateFcn();

        /* Update old values */
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
