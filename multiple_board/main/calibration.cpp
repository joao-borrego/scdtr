/** 
* @file calibration.cpp
* @brief Calibration utility implementation
* @author João Borrego
* @author António Almeida
*/

#include "calibration.hpp"

namespace Calibration{

    /** Allows a slave to progress */
    volatile bool calibration_ready{false};
    /** $a$ parameter of ldr characteristic */
    float lux_a{0};
    /** $b$ parameter of ldr characteristic */
    float lux_b{0};

    /** */
    volatile int idx{0};
    /** */
    volatile float tmp_lux[N * 2]; 

    void writeFloat(float var){
        int size = sizeof(float);
        byte packet[size];
        float2bytes_t f2b;
        f2b.f = var;
        for (int i = 0; i < size; i++)
            packet[i] = f2b.b[i];
        Wire.write(packet, size);
    }

    float readFloat(){
        float2bytes_t b2f;
        bool received = false;
        int i = 0;

        while (Wire.available()) { 
            b2f.b[i++] = Wire.read();
            received = true;
        }
        return (received)? b2f.f : -1;
    }

    void inline sendReady(int destination){
        byte value = 0;
        Wire.beginTransmission(destination);
        Wire.write(value);
        Wire.endTransmission();
    }

    void inline waitReady(){
        while (calibration_ready == false){}
        calibration_ready = false;
    }

    void inline barrier(int id){
        if (id == MASTER){
            for (int i = 0; i < N; i++){
                if (i != MASTER){
                    sendReady(i);
                }
            }
        } else {
            waitReady();
        }
    }

    float getLDRValue(){
        float ldr_in = analogRead(pin_ldr);
        float v_in = ldr_in * (VCC / 1023.0);
        float input = Utils::convertToLux(v_in, lux_a, lux_b);
        return input;
    }

    void onReceive(int bytes){

        if (Wire.available() != 0){
            if (bytes == 1){
                byte value = Wire.read();
                if (value == READY){
                    calibration_ready = true;
                }
            }
        }
    }

    void onRequest(){
        float lux = getLDRValue();
        tmp_lux[idx++] = lux;
        writeFloat(lux);
    }

    void execute(float *k_i, float *o_i, uint8_t id){

        float output[2] = {127.0, 255.0};
        
        float value;

        lux_a = LUX_A[id];
        lux_b = LUX_B[id];

        *o_i = getLDRValue();

        Serial.println("[Calibration] Started");
        barrier(id);

        for (int s = 0; s < 2; s++){
            
            for (int i = 0; i < N; i++){
                // Turn system i led on
                if (i == id){
                    analogWrite(pin_led, (int) output[s]);
                }
                delay(300);

                if (id == MASTER){
                    for (int j = 0; j < N; j++){
                        if (j == MASTER){
                            value = getLDRValue();
                            tmp_lux[idx++] = value;
                        } else {
                            Wire.requestFrom(j, 4);
                            float tmp;
                            while ((tmp = readFloat()) < 0){}
                            //Serial.println(tmp);
                        }
                    }
                }

                barrier(id);

                // Turn system i led off
                if (i == id){
                    analogWrite(pin_led, 0);
                }
                delay(100);
            }
        }

        Serial.print("o_i = ");
        Serial.println(*o_i, 3);

        Serial.print("k_i = [");
        for (int j = 0; j < N; j++){
            k_i[j] = (tmp_lux[N + j] - tmp_lux[j]) / (output[1] - output[0]) * (255.0 / 100.0);
                Serial.print(k_i[j], 3);
                Serial.print(" ");
        }
        Serial.println("]");

        Serial.println("[Calibration] Done");
    }
}
