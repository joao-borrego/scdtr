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
        const float k_p,
        const float k_i,
        const float T,
        const float b,
        const float k_sat,
        const float anti_windup_sat_min,
        const float anti_windup_sat_max,
        const float error_deadzone_min,
        const float error_deadzone_max){

        this->in = input;
        this->out = output;
        this->ref = reference;
        this->k_p = k_p;
        this->k_i = k_i;
        this->T = T;

        this->b = b;
        this->k_sat = k_sat;
        this->sat_min = anti_windup_sat_min;
        this->sat_max = anti_windup_sat_max;
        this->deadzone_min = error_deadzone_min;
        this->deadzone_max = error_deadzone_max;
        
        this->err = 0.0;
        this->i = 0.0;
        this->u = 0.0;
        this->u_sat = 0.0;
        this->y = 0.0;

        this->use_feedforward = false;
        this->use_anti_windup = false;
        this->use_deadzone = false;

        //this->k_1 = k_p * b;
        this->k_2 = k_p * k_i * T / 2.0;
    }

    Controller::configureFeatures(
        const bool use_feedforward,
        const bool use_anti_windup,
        const bool use_deadzone){

        this->use_feedforward = use_feedforward;
        this->use_anti_windup = use_anti_windup;
        this->use_deadzone = use_deadzone;
    }

    void Controller::update(void (*updateFcn)(void)){

        /* Sample input and reference */
        float y_sampled = *(in);
        float ref_sampled = *(ref);
        float err_new = ref_sampled - y_sampled;
        
        /* Apply error deadzone to reduce output jitter */
        if (use_deadzone){
            err_new = applyDeadzone(err_new);
        }

        float p_new{0}, i_new{0}, u_new{0}, u_sat_new{0};
        
        p_new = k_p * err_new;
        if (use_feedforward){
            // Add feedforward term
            p_new += b * getFeedforward(ref_sampled);
        }

        //p_new = k_1 * ref_sampled - k_p * y;

        i_new = i + k_2 * (err_new + err);
        if (use_anti_windup){
            /* Simple anti-windup only saturates the integrator term */
            #if SIMPLE_ANTI_WINDUP
                i_new = applySaturation(i_new);
                u_new = u_new = p_new + i_new;
                *(this->out) = u_new;
            /* Real anti-windup has a feedback loop */
            #else
                i_new += k_sat * (u_sat - u);
                u_new = p_new + i_new;
                u_sat_new = applySaturation(u_new);
                *(this->out) = u_sat_new;
            #endif
        } else {
            u_new = p_new + i_new;
            *(this->out) = u_new;
        }

        /* Update output using external callback function */
        updateFcn();

        /* Update old values */
        this->y = y_sampled;
        this->err = err_new;
        this->i = i_new;
        this->u = u_new;
        # if !SIMPLE_ANTI_WINDUP
            if (use_anti_windup){
                this->u_sat = u_sat_new;
            }
        #endif
    }

    inline float Controller::applySaturation(float output){
        if (output > sat_max){
            return sat_max;
        } else if (output < sat_min){
            return sat_min;
        }
        return output;
     }

    inline float Controller::applyDeadzone(float error){
        if (error > deadzone_max){
           return  error - deadzone_max;
        } else if (error < deadzone_min){
            return error - deadzone_min;
        }
        return 0.0;
    }

    inline float Controller::getFeedforward(float y){
        // y = m * x + b
        return (y - B_FF) / M_FF;
    }

    /* Getters and setters */

    void Controller::useFeedforward(bool state){
        this->use_feedforward = state;
    }

    void Controller::useAntiWindup(bool state){
        this->use_anti_windup = state;
    }

    void Controller::useErrorDeadzone(bool state){
        this->use_deadzone = state;
    }
    
    /* Unused functions */

    void Controller::setAntiWindupSat(float sat_min, float sat_max){
        this->sat_min = sat_min;
        this->sat_max = sat_max;
    }

    void Controller::setErrorDeadzone(float deadzone_min, float deadzone_max){
        this->deadzone_min = deadzone_min;
        this->deadzone_max = deadzone_max;
    }

    void Controller::updateCoefficients(
        float k_p,
        float k_i){

        this->k_p = k_p;
        this->k_i = k_i;
        /* Update internal constants */
        //this->k_1 = k_p * b;
        this->k_2 = k_p * k_i * T / 2.0;

        Controller::resetHistory();
    }

    inline void Controller::resetHistory(){
        this->y = 0;
        this->err = 0;
        this->i = 0;
    }

}
