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

    /* Pointers to important variables */

    /** Reset flag pointer */
    volatile bool *reset;
    /** Start consensus flag pointer */
    volatile bool *consensus;
    /** Illuminance lower bound pointer */
    volatile float *lower_bound;


    /* Functions */

    void nop(int bytes){}

    void setup(
        const uint8_t id,
        const bool *reset_ptr,
        const bool *consensus_ptr,
        const float *lower_bound_ptr)
    {
        dev_id = id;
        reset = reset_ptr;
        consensus = consensus_ptr;
        lower_bound = lower_bound_ptr;
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

    void sendPacket(uint8_t dest, uint8_t type)
    {
        byte packet[HEADER_SIZE];
        packet[ID] = dev_id;
        packet[TYPE] = type;
        
        Wire.beginTransmission(dest);
        Wire.write(packet, HEADER_SIZE);
        Wire.endTransmission();

        Serial.println("[I2C] Sending header-only packet");
    }

    void sendConsensus(uint8_t dest, bool start, float *d_i)
    {
        size_t size = HEADER_SIZE + DATA_CONSENSUS_SIZE;
        byte packet[size];
        packet[ID] = dev_id;
        packet[TYPE] = (start)? ICO : CON;
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

        if (start)  Serial.println("[I2C] Sending initiate consensus packet");
        else        Serial.println("[I2C] Sending consensus packet");
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
        packet[ID] = dev_id;
        packet[TYPE] = INF;
        
        float_bytes fb;
        fb.f = lux;
        uint8_t occupied = (occupancy)? 1 : 0;
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
        packet[2 + 5 * sizeof(float)] = occupied;
        
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
                    *reset = true;
                    break;
                case ICO:
                    // Update lower bound
                    float_bytes fb;
                    for (int i = 0; i < sizeof(float); i++){
                        fb.b[i] = buffer[i + dev_id * sizeof(float)];    
                    }
                    *lower_bound = fb.f;
                    *consensus = true;
                    break;
            }
        }
    }
}
