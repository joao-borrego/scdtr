/** 
 * @sweep.ino
 * @brief System calibration tool
 * @author António Almeida
 * @author João Borrego
 */

#include "utils.hpp"

/** Baudrate value for Serial communication */ 
#define BAUDRATE 115200

/** Minimum value for the reference */
#define MIN_reference 0
/** Maximum value for the reference */
#define MAX_reference 100

/** Whether to use lookup table or linear approximation */
#define USE_LOOKUP true

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

    // sweep(min, max, step, sample_time, samples, convert)

    //sweep(0, 255, 50, 1, 1, false);
    sweep(0, 255, 1, 50, 1, true);
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
            new_output = findLinReg(reference);
               
        #endif
        analogWrite(pin_led, new_output);

        delay(500);
        ldr_in =  analogRead(pin_ldr);
        v_in = ldr_in * (5.0 / 1023.0);
        lux_value = Utils::convertToLux(v_in);
        Serial.println(lux_value);
    }
}

/**
 * @brief      Performs a sweep on the output pin
 *
 * @param[in]  min          The minimum value
 * @param[in]  max          The maximum value
 * @param[in]  step         The step
 * @param[in]  sample_time  The sample time
 * @param[in]  samples      The number of samples
 * @param[in]  convert      Whether to convert units to LUX
 */
void sweep(
    int min,
    int max,
    int step,
    int sample_time,
    int samples,
    bool convert){

    for (int out = min; out <= max; out += step){
        analogWrite(pin_led, out);
        for (int i = 0; i < samples; i++){
            ldr_in =  analogRead(pin_ldr);
            if (convert){
                v_in = ldr_in * (5.0 / 1023.0);
                lux_value = Utils::convertToLux(v_in);
                Serial.print(lux_value);
            } else {
                Serial.print(ldr_in);
            }
            Serial.print(", ");
            delay(sample_time);
        }
    }
    Serial.println();
}
