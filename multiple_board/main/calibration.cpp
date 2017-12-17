/**
* @file calibration.cpp
* @brief Calibration utility implementation
* @author João Borrego
* @author António Almeida
*/

#include "calibration.hpp"

namespace Calibration
{

    /* Global variables */

    /** Node identifier */
    int id;
    /** Lock progression */
    volatile bool ready;
    /** Current state*/
    volatile int  state;
    /** Temporary copy of external lux */
    volatile float y_ext;
    /** Registered lux values */
    volatile float Y[N][NUM_SAMPLES];

    int sample;
    uint8_t sender;
    uint8_t receiver;

    /* Functions */

    void onReceive(int bytes){

        byte p_id, p_type;
        size_t p_size = bytes;

        byte packet[MAX_SIZE];
        Communication::readPacket(&p_id, &p_type, p_size, packet);

        if (id == sender)
        {
            if (p_id == receiver && p_type == ACK){
                ready = true;
            }
        }
        else
        {
            if (p_id == sender && p_type == SYN){
                if (state == STATE_EXT)
                {
                    y_ext = getLDRValue();
                }
                else if (state == STATE_ON)
                {
                    Y[sender][sample] = getLDRValue();
                    state = STATE_OFF;
                }
                else
                {
                    state = STATE_ON;
                }
                ready = true;
            }
        }
    }

    float getLDRValue(){
        float ldr_in = analogRead(pin_ldr);
        float v_in = ldr_in * (VCC / 1023.0);
        float input = Utils::convertToLux(v_in, LUX_A[id], LUX_B[id]);
        return input;
    }

    void execute(int id_, float *k_i, float *o_i){

        float X[NUM_SAMPLES] = SAMPLES;

        id      = id_;
        ready   = false;
        state   = STATE_EXT;
        sender  = 0;

        // Calculate external iluminance
        analogWrite(pin_led, 0);
        delay(WAIT_LED);
        if (id == MASTER){
            y_ext = getLDRValue();
            for (receiver = 0; receiver < N; receiver++){
                if (receiver != MASTER){
                    while (!ready){
                        Communication::sendPacket(receiver, SYN);
                        delay(WAIT);
                    }
                    ready = false;
                }
            }

        } else {
            while (!ready) { }
            ready = false;
            Communication::sendPacket(sender, ACK);
        }
        state = 1;

        *o_i = y_ext;

        if (id != MASTER){
            Serial.print("o_i = ");
            Serial.println(y_ext, 5);
        }

        // Obtain data points
        for (sample = 0; sample < NUM_SAMPLES; sample++){
            for (sender = 0; sender < N; sender++){
                if (id == sender){
                    analogWrite(pin_led, X[sample]);
                    delay(WAIT_LED);
                    Y[sender][sample] = getLDRValue();
                    for (receiver = 0; receiver < N; receiver++){
                        if (sender != receiver){
                            while(!ready){
                                Communication::sendPacket(receiver, SYN);
                                delay(WAIT);
                            }
                            ready = false;
                        }
                    }
                    analogWrite(pin_led, 0);
                    delay(WAIT_LED);
                    for (receiver = 0; receiver < N; receiver++){
                        if (sender != receiver){
                            while(!ready){
                                Communication::sendPacket(receiver, SYN);
                                delay(WAIT);
                            }
                            ready = false;
                        }
                    }
                } else {
                    while (!ready) { }
                    ready = false;
                    Communication::sendPacket(sender, ACK);
                    while (!ready) { }
                    ready = false;
                    Communication::sendPacket(sender, ACK);
                }
            }
        }

        // Calculate linear regression for each element of K
        // using least squares method

        float x_avg = Utils::average(X, NUM_SAMPLES);

        for (int i = 0; i < N; i++){
            float y_avg = Utils::average(Y[i], NUM_SAMPLES);

            float num = 0.0;
            float den = 0.0;

            for (int sample = 0; sample < NUM_SAMPLES; sample++){
                num += (X[sample] - x_avg) * (Y[i][sample] - y_avg);
                den += (X[sample] - x_avg) * (X[sample] - x_avg);
            }

            k_i[i] = num / den * (255.0 / 100.0);
        }

        if (id != MASTER){
            Serial.print("k_i = [");
            for (int j = 0; j < N; j++){
                Serial.print(k_i[j], 5);
                Serial.print(" ");
            }
            Serial.println("]");
        }
    }
}
