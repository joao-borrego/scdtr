/** 
 * @file sweep.ino
 * @brief System calibration tool
 * @author António Almeida
 * @author João Borrego
 */

#include "sweep_utils.hpp"

/** Baudrate value for Serial communication */ 
#define BAUDRATE 115200

/** Minimum value for the reference */
#define MIN_reference 0
/** Maximum value for the reference */
#define MAX_reference 100

/* Units */
#define RAW   0
#define VOLT  1
#define LUX   2

/** Whether to use lookup table or linear approximation */
#define USE_LOOKUP false

/** LDR analog in pin */
int pin_ldr = A0;
/** LED PWM out pin */
int pin_led = 11;

/* Global variables */

/** Lookup table */
float lookup[256] = LOOKUP_TABLE;

/** Linear regresion parameters */
float a = 0.4507;
float b = -5.2925;

float reference {0};
float ldr_in {0};
float v_in {0};
float lux_value {0};
float new_output {0};

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

    /* f) convert previous plotss to LUX */
    //sweep(0, 255, 10, 0.2, 500, LUX);
    //sweep(0, 255, 1, 50, 1, LUX);

    /* Calculate time constant (impulse response w/ pwm at 128) */
    sweep(0, 128, 128, 0.1, 1000, VOLT);
    
    /* OLD */
    //sweep(0, 255, 50, 1, 1, false);
    //sweep(0, 255, 1, 50, 1, true);
    //sweep(0, 255, 10, 50, 1, false);

    // Set led pin to 0 and wait enough time to discharge capacitor
    analogWrite(pin_led, 0);
    delay(5000);

    /* Feedforward controller interactive test */

    //feedForward();
}

int findLookupTable(float reference){
    for (int i = 1; i < 256; i++){
        if (reference < lookup[i-1]){
            new_output = i-1;
            Serial.print("Target led value is ");
            Serial.println(lookup[i-1]);
            return i-1;
        }
    }
}

int findLineReg(float reference){
    // y = a*x + b
    return (int) (reference - b) / a;
}

/**
 * @brief      Simple feedforward controller for testing
 */
void feedForward(){

    while (Serial.available() == 0){}

    reference = Serial.parseFloat();
    if (reference < MIN_reference || reference > MAX_reference){
        Serial.println("Invalid value provided!");
    } else {

        #if USE_LOOKUP == true
            new_output = findLookupTable(reference);
        #else
            new_output = findLineReg(reference);     
        #endif

        analogWrite(pin_led, new_output);

        for (int i = 0; i < 200; i++){
            ldr_in =  analogRead(pin_ldr);
            v_in = ldr_in * (5.0 / 1023.0);
            lux_value = Utils::convertToLux(v_in);
            Serial.print(lux_value);
            Serial.print(", ");
            delay(1);
        }
        Serial.println();
    }
}

/**
 * @brief      Performs a sweep on the output pin
 *
 * @param[in]  min          The minimum value
 * @param[in]  max          The maximum value
 * @param[in]  step         The step
 * @param[in]  sample_time  The sample time [ms]
 * @param[in]  samples      The number of samples
 * @param[in]  unit         The unit of the output
 */
void sweep(
    int min,
    int max,
    int step,
    float sample_time,
    int samples,
    int unit){

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
            delayMicroseconds(1000.0 * sample_time);
        }
    }
    Serial.println();
}
