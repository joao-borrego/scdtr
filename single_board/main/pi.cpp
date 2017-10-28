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
        volatile float *input,
        volatile float *output,
        volatile float *reference,
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
        
        this->force_y = -1;
        this->use_deadzone = false;
        this->use_anti_windup = false;

        this->k_1 = k_p * b;
        this->k_2 = k_p * k_i * T / 2.0;
    }

    void Controller::update(void (*updateFcn)(void)){

        /* Check if an output value was forced */
        if (force_y > 0){
            *(this->out) = force_y;
            updateFcn();
            Controller::resetHistory();
            this->y = force_y;
            force_y = -1;
            return;
        }

        /* Perform calculations */

        float y_new = *(in);
        float new_ref = *(ref);
        float err_new = new_ref - y_new;
        
        /* Apply (possibly asymmetric) deadzone to reduce output jitter */
        if (use_deadzone){
            if (err_new >= deadzone_max){
                err_new -= deadzone_max;
            } else if (err_new <= deadzone_min){
                err_new -= deadzone_min;
            } else {
                err_new = 0;
            }
        }

        float p_new, i_new;
        p_new = k_1 * new_ref - k_p * y;
        i_new = i + k_2 * (err_new + err);

        /* TODO Perform proper anti-windup */
        if (use_anti_windup){
            if (i_new < ANTI_WINDUP_SAT_MIN){
                i_new = ANTI_WINDUP_SAT_MIN;
            } else if (i_new > ANTI_WINDUP_SAT_MAX){
                i_new = ANTI_WINDUP_SAT_MAX;
            }
        }

        *(this->out) = p_new + i_new;

        /* Update output using external callback function */
        updateFcn();

        /* Update old values */
        this->y = y_new;
        this->err = err_new;
        this->p = p_new;
        this->i = i_new;
    }

    void Controller::updateCoefficients(
        float k_p,
        float k_i){

        this->k_p = k_p;
        this->k_i = k_i;
        /* Update internal constants */
        this->k_1 = k_p * b;
        this->k_2 = k_p * k_i * T / 2.0;

        Controller::resetHistory();
    }

    inline void Controller::resetHistory(){
        this->y = 0;
        this->err = 0;
        this->p = 0;
        this->i = 0;
    }

    void Controller::setErrorDeadzone(float deadzone_min, float deadzone_max){
        this->deadzone_min = deadzone_min;
        this->deadzone_max = deadzone_max;
    }

    void Controller::setAntiWindupSat(float sat_min, float sat_max){
        this->sat_min = sat_min;
        this->sat_max = sat_max;
    }

    void Controller::useErrorDeadzone(bool state){
        this->use_deadzone = state;
    }

    void Controller::useAntiWindup(bool state){
        this->use_anti_windup = state;
    }
}
