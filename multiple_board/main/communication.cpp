/**
 * @file communication.cpp
 * 
 * @brief Communication protocol
 */

#include <Arduino.h>
#include <Wire.h>

#include "communication.hpp"

namespace Communication
{
    /* Global variables */

    /** Device Id */
    uint8_t dev_id = 0;

    /* Barrier lock */
    volatile bool lock = false;

    /* For handles */

    /** Reset flag */
    volatile bool reset     = false;
    /** Start consensus flag */
    volatile bool consensus = false;

    /* Functions */

    void nop(int bytes){}

    void setDeviceId(uint8_t id)
    {
        dev_id = id;
    }

    void barrier(uint8_t id)
    {
        Serial.println("[Barrier] Start");
        if (id == MASTER){
            for (int i = 0; i < N; i++){
                if (i != MASTER){
                    Wire.requestFrom(i, 1);
                    while(waitAck());
                }
            }
        } else {
            waitSyn();
        }
        Serial.println("[Barrier] End");
    }

    void waitSyn()
    {
        while (lock == false){}
        lock = false;
    }

    bool waitAck()
    {
        while (Wire.available()){
            if (Wire.read()) return false;
        }
        return true;
    }

    void onRequest()
    {
        Wire.write(ACK);
        lock = true;
    }

    void sendAck(uint8_t dest)
    {
        byte packet[HEADER_SIZE];
        packet[0] = dev_id;
        packet[1] = ACK;
        
        Wire.beginTransmission(dest);
        Wire.write(packet, HEADER_SIZE);
        Wire.endTransmission();

        Serial.println("[I2C] Sending ACK");
    }

    void sendConsensus(uint8_t dest, float *d_i)
    {
        size_t size = HEADER_SIZE + DATA_CONSENSUS_SIZE;
        byte packet[size];
        packet[0] = dev_id;
        packet[1] = CON;
        for (int j = 0; j < N; j++)
        {
            float_bytes d;
            d.f = d_i[j];
            for (int k = 0; k < sizeof(float); k++){
                packet[2 + j * sizeof(float) + k] = d.b[k];    
            }
        }

        Wire.beginTransmission(dest);
        Wire.write(packet, size);
        Wire.endTransmission();
    }

    void sendInfo(
        uint8_t dest,
        float lux,
        float duty_cycle,
        float lower_bound,
        float ext,
        float ref,
        bool occupancy)
    {
        size_t size = HEADER_SIZE + DATA_INFO_SIZE;
        byte packet[size];
        packet[0] = dev_id;
        packet[1] = INF;
        
        float_bytes fb;
        fb.f = lux;
        for (int j = 0; j < sizeof(float); j++){
            packet[2 + j] = fb.b[j];    
        }
        fb.f = duty_cycle;
        for (int j = 0; j < sizeof(float); j++){
            packet[2 + sizeof(float) + j] = fb.b[j];    
        }
        fb.f = lower_bound;
        for (int j = 0; j < sizeof(float); j++){
            packet[2 + 2 * sizeof(float) + j] = fb.b[j];    
        }
        fb.f = ext;
        for (int j = 0; j < sizeof(float); j++){
            packet[2 + 3 * sizeof(float) + j] = fb.b[j];    
        }
        fb.f = ref;
        for (int j = 0; j < sizeof(float); j++){
            packet[2 + 4 * sizeof(float) + j] = fb.b[j];    
        }
        packet[2 + 5 * sizeof(float)] = (occupancy)? 0 : 1;
        
        Wire.beginTransmission(dest);
        Wire.write(packet, size);
        Wire.endTransmission();
    }

    void readConsensus(byte *buffer, float *out){

        for (int j = 0; j < N; j++){
            float_bytes d;
            for (int k = 0; k < sizeof(float); k++){
                d.b[k] = buffer[k + j * sizeof(float)];    
            }
            out[j] = d.f;
        }
    }

    size_t readToBuffer(byte *buffer){

        size_t read = 0;
        while (Wire.available()) {
            buffer[read++] = Wire.read();
        }
        return read;
    }

    void onReceive(int bytes)
    {
        byte buffer[MAX_SIZE] = {0};
        size_t read = 0;

        while (Wire.available()) {
            buffer[read++] = Wire.read();
        }

        if (read > 0){
            
            // DEBUG
            Serial.print("[I2C] Received ");
            Serial.print(read);
            Serial.print(" bytes from ");
            Serial.println(buffer[ID]);
            
            uint8_t type = buffer[TYPE];
            switch (type)
            {
                case RES:
                    reset = true;
                    break;
                case ICO:
                    consensus = true;
                    break;
            }
        }
    }
}
