/** 
 * @file sweep.ino
 * @brief System calibration tool
 * @author António Almeida
 * @author João Borrego
 */

#include "sweep_utils.hpp"

/** Baudrate value for Serial communication */ 
const unsigned long BAUDRATE = 115200;

/* Units */
const int RAW   = 0;
const int VOLT  = 1;
const int LUX   = 2;

/** LDR analog in pin */
const int pin_ldr = A0;
/** LED PWM out pin */
const int pin_led = 11;

/* Global variables */

/** Linear regresion parameters */
const float m = 0.4330;  // 0.4507;
const float b = -4.6204; // -5.2925;

float reference {0};
float ldr_in {0};
float v_in {0};
float lux_value {0};

/**
 * @brief      Arduino setup
 */
void setup() {
    /* Setup serial communication */
    Serial.begin(BAUDRATE);
}

/**
 * @brief      Arduino Loop
 */
void loop() {

    /* Calibration sampling routine */

    // sweep(min, max, step, sample_time, samples, unit)

    /* d) v_ldr(pwm) T = 100 ms */
    //sweep(0, 255, 1, 100, 1, VOLT);
    //sweep(0, 255, 10, 100, 1, VOLT);
    //sweep(0, 255, 10, 0.2, 500, VOLT);

    /* e) Steady-state raw_in(pwm) T = 50 ms */
    //sweep(0, 255, 1, 50, 1, RAW);

    /* f) convert previous plots to LUX */
    //sweep(0, 255, 10, 0.2, 500, LUX);
    sweep(0, 255, 1, 50, 1, LUX);

    /* Calculate time constant (impulse response w/ pwm at 128) */
    //sweep(0, 128, 128, 0.5, 300, VOLT);
    //sweep(0, 128, 128, 0.5, 300, LUX);

    // Set led pin to 0 and wait enough time to discharge capacitor
    analogWrite(pin_led, 0);
    delay(5000);
}

/**
 * @brief      Performs a sweep on the output pin
 *
 * @param      min          The minimum value
 * @param      max          The maximum value
 * @param      step         The step
 * @param      sample_time  The sample time [ms]
 * @param      samples      The number of samples
 * @param      unit         The unit of the output
 */
void sweep(
    int min,
    int max,
    int step,
    float sample_time,
    int samples,
    int unit){

    /** Latest elapsed microseconds since startup */
    unsigned long current_micros {0};
    /** Previously recorded elapsed microseconds since startup */
    unsigned long last_micros {0};
    /** Minimum delay between samples in microseconds */
    unsigned long sample_period = 1000.0 * sample_time;

    for (int out = min; out <= max; out += step){
        analogWrite(pin_led, out);
        for (int i = 0; i < samples; i++){
            ldr_in =  analogRead(pin_ldr);
            switch (unit) {
                case(RAW):
                    Serial.print(ldr_in);
                    break;  
                case(VOLT):
                    v_in = ldr_in * (5.0 / 1023.0);
                    Serial.print(v_in);
                    break;
                case(LUX):
                    v_in = ldr_in * (5.0 / 1023.0);
                    lux_value = Utils::convertToLux(v_in);
                    Serial.print(lux_value);
            }
            Serial.print(", ");

            /* Wait for the sample period in us */
            while (current_micros - last_micros < sample_period){
                current_micros = micros();
            }
            last_micros = current_micros;
        }
    }
    Serial.println();
}
