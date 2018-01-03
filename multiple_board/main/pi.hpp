/**
 * @file    multiple_board/main/pi.cpp
 * @brief   PI Controller headers
 *
 * Proportional Integral Controller class headers
 *
 * @author  João Borrego
 * @author  António Almeida
 */

#ifndef INO_PI_HPP
#define INO_PI_HPP

#include "Arduino.h"

/** Whether to just saturate the integrator term or to use a proper 
 anti-windup feedback loop and saturate the u output */
#define SIMPLE_ANTI_WINDUP false

namespace PIController {

    /** Default feedforward gain */
    const float K_FF                 =  0.5; // 1.0;
    /** Feedforward approximated linear characteristic m parameter */
    const float M_FF                 =  0.4330;
    /** Feedforward approximated linear characteristic b parameter */
    const float B_FF                 =  -4.6204;
    /** Default anti-windup loop gain */
    const float K_SAT                =  1.0;
    /** Default anti-windup negative saturation threshold */
    const float ANTI_WINDUP_SAT_MIN  =  0.0;
    /** Default anti-windup positive saturation threshold */
    const float ANTI_WINDUP_SAT_MAX  =  255.0;
    /** Default error deadzone minimum threshold */
    const float ERR_DEADZONE_MIN     =  -4.0;
    /** Default error deadzone maximum threshold */
    const float ERR_DEADZONE_MAX     =  4.0;

    /**
     * @brief      PI Controller class with additional features for smoother performance.
     * 
     * A simple PI Controller with parallel design in order to decouple the proportional and
     * integral gains.
     * It has 3 main optional optimisations:
     * 
     * 1. Feedforward controller term to quickly reach reference
     * 2. Anti-windup saturation (with and without feedback loop)
     * 3. Error deadzone to reduce output jitter
     * 
     */
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

        /** Error with respect to the reference */
        float err;
        /** Integral term */
        float i;
        /** Output of the controller */
        float u;
        /** Saturated output of the controller */
        float u_sat;
        /** Input of the controller */
        float y;

        /** Sampling time */
        float T;

        /* Internal flags */

        /** Whether to use feedforward term */
        bool use_feedforward;
        /** Whether to use anti-windup */
        bool use_anti_windup;
        /** Whether to use deadzone */
        bool use_deadzone;

        /* Numerical constants */

        /** Feedforward weight gain */
        float b;
        /** Anti-windup loop gain */
        float k_sat;
        /** Anti-windup negative saturation threshold */
        float sat_min;
        /** Anti-windup positive saturation threshold */
        float sat_max;
        /** Error deadzone minimum threshold */
        float deadzone_min;
        /** Error deadzone maximum threshold */
        float deadzone_max;

        /**
         * k_2 = k_p * k_i * T / 2
         * i = i_old + (k_p * k_i * T / 2) * (err - err_old)
         */
        float k_2;

    public:

        /**
         * @brief      Constructs the PI controller object.
         *
         * @param      input                The pointer to the input
         * @param      output               The pointer to the output
         * @param      reference            The pointer to the reference
         * @param      k_p                  The coefficient for the proportional term
         * @param      k_i                  The coefficient for the integral term
         * @param      T                    Sapling time
         * @param      b                    Feedforward controller gain
         * @param      k_sat                The anti-windup loop gain
         * @param      anti_windup_sat_min  The anti-windup sat minimum
         * @param      anti_windup_sat_max  The anti-windup sat maximum
         * @param      error_deadzone_min   The error deadzone minimum
         * @param      error_deadzone_max   The error deadzone maximum
         */
        Controller(
            volatile float *input,
            volatile float *output,
            volatile float *reference,
            const float k_p,
            const float k_i,
            const float T,
            const float b = K_FF,
            const float k_sat = K_SAT,
            const float anti_windup_sat_min = ANTI_WINDUP_SAT_MIN,
            const float anti_windup_sat_max = ANTI_WINDUP_SAT_MAX,
            const float error_deadzone_min = ERR_DEADZONE_MIN,
            const float error_deadzone_max = ERR_DEADZONE_MAX);

        /**
         * @brief      Configures which controller features should be enabled
         *
         * @param      use_feedforward  Whether to use feedforward
         * @param      use_anti_windup  Whether to use anti-windup
         * @param      use_deadzone     Whether to use error deadzone
         */
        configureFeatures(
            const bool use_feedforward,
            const bool use_anti_windup,
            const bool use_deadzone);

        /**
         * @brief      Updates output
         *
         * @param      updateFcn    The pointer to the output callback fuction
         */
        void update(void (*updateFcn)(void));

        /**
         * @brief      Applies the anti-windup loop saturation
         *
         * @param[in]  output  The output before saturation
         *
         * @return     The saturated output
         */
        inline float applySaturation(float output);

        /**
         * @brief      Applies the error deadzone
         *
         * @param      error  The error
         *
         * @return     Returns the processed error
         */
        inline float applyDeadzone(float error);

        /**
         * @brief      Gets the feedforward estimate
         *
         * Uses an approximated linear characteristic to obtain
         * the u output value corresponding to a given reference
         * y value.
         *
         * @param      y     The desired reference value
         *
         * @return     The corresponding output value
         */
        inline float getFeedforward(float y);

        /**
         * @brief      Sets whether the controller should use the feedforward estimate.
         *
         * @param      state  The desired state
         */
        void useFeedforward(bool state);

        /**
         * @brief      Sets whether the controller should use the error deadzone.
         *
         * @param      state  The desired state
         */
        void useErrorDeadzone(bool state);

        /**
         * @brief      Sets whether the controller should use the anti-windup mechanism.
         *
         * @param      state  The desired state
         */
        void useAntiWindup(bool state);
        
        /**
         * Unused functions
         */

        /**
         * @brief      Updates internal coefficients
         *
         * @param      k_p   The k p
         * @param      k_i   The k i
         */
        void updateCoefficients(float k_p, float k_i);

        /**
         * @brief      Resets controller history.
         * 
         * Resets the internal variables that account for the past.
         */
        inline void resetHistory();

        /**
         * @brief      Sets the error deadzone.
         *
         * @param      deadzone_min  The deadzone minimum
         * @param      deadzone_max  The deadzone maximum
         */
        void setErrorDeadzone(float deadzone_min, float deadzone_max);

        /**
         * @brief      Sets the anti windup saturation parameters.
         *
         * @param      sat_min  The saturation minimum
         * @param      sat_max  The saturation maximum
         */
        void setAntiWindupSat(float sat_min, float sat_max);
    };
}

#endif
