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

    float getLDRValue(){
        float ldr_in = analogRead(pin_ldr);
        float v_in = ldr_in * (VCC / 1023.0);
        float input = Utils::convertToLux(v_in, lux_a, lux_b);
        return input;
    }

    void onReceive(int bytes){

        if (Wire.available() != 0){
            // Command
            if (bytes == 1){
                byte value = Wire.read();
                if (value == 0){
                    calibration_ready = true;
                }
            // Data
            } else {
                for (int i = 0; i < bytes; i++){
                    byte value = Wire.read();
                    Serial.print("[I2C] Received: ");
                    Serial.println(value, HEX);
                }
            }
        }
    }

    void onRequest(){
        float lux = getLDRValue();
        writeFloat(lux);
    }

    void execute(float *k_matrix, int devices, uint8_t id){

        float value;
        float output[2] = {127.0, 255.0};
        float tmp[N*N*2];

        lux_a = LUX_A[id];
        lux_b = LUX_B[id];

        // Master 
        if (id == MASTER){
            for (int i = 0; i < devices; i++){
                if (i != MASTER){
                    sendReady(i);
                }
            }
        } else {
            waitReady();
        }

        Serial.println("[Calibration] Started");

        for (int s = 0; s < 2; s++){
            
            for (int i = 0; i < devices; i++){
                // Turn system i led on
                if (i == id){
                    analogWrite(pin_led, (int) output[s]);
                }
                delay(100);

                if (id == MASTER){
                    for (int j = 0; j < devices; j++){
                        if (j == MASTER){
                            value = getLDRValue();
                        } else {
                            Wire.requestFrom(j, 4);
                            while ((value = readFloat()) < 0){}
                        }
                        
                        // DEBUG
                        /*
                        Serial.print(value);
                        if (i == j && j == devices - 1){
                            Serial.print("\n");
                        } else {
                            Serial.print(",");
                        }
                        */

                        tmp[i*N + j*N + s] = value; 
                    }
                    for (int j = 0; j < devices; j++){
                        if (j != MASTER){
                            sendReady(j);
                        }
                    }
                } else {
                    waitReady();
                }

                // Turn system i led off
                if (i == id){
                    analogWrite(pin_led, 0);
                }
                delay(100);
            }
        }

        
        if (id == MASTER) {
            Serial.print("K = [");
            for (int i = 0; i < devices; i++){
                for (int j = 0; j < devices; j++){
                    k_matrix[i*devices + j] = (tmp[i*devices + j*devices + 1] - tmp[i*devices + j*devices]) / 
                        (output[1] - output[0]);
                    Serial.print(k_matrix[i*devices + j]);
                    Serial.print(" ");
                }
            }
            Serial.print("]\n");

            // TODO - Send matrix to each arduino?
        }

        Serial.println("[Calibration] Done");
    }
}