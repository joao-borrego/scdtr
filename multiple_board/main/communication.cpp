/**
 * @file    multiple_board/main/communication.cpp
 * 
 * @brief   I2C communication protocol implementation
 * 
 * Defines I2C communication protocol.
 * Implements functions to send supported packets.
 * 
 * @author  João Borrego
 * @author  António Almeida
 */

#include <Arduino.h>
#include <Wire.h>

#include "communication.hpp"

namespace Communication
{
    /* Global variables */

    /** Device Id */
    uint8_t *dev_id;

    /** Barrier lock */
    volatile bool lock = false;

    /* Pointers to important variables */

    /** Reset flag pointer */
    volatile bool *reset        {NULL};
    /** Occupancy changed pointer */
    volatile bool *changed      {NULL};
    /** Illuminance reference pointer */
    volatile float *ref         {NULL};
    /** Illuminance lower bound pointer */
    volatile float *lower_bound {NULL};
    /** Occupancy pointer */
    volatile bool *occupancy    {NULL};
    /** Use distributed control flag pointer */
    volatile bool *distributed  {NULL};

    /* Functions */

    void nop(int bytes)
    {
        // No OPeration
    }

    void setup(
        const uint8_t *id,
        const bool  *reset_ptr,
        const bool  *changed_ptr,
        const float *ref_ptr,
        const float *lower_bound_ptr,
        const bool  *occupancy_ptr,
        const bool  *distributed_ptr)
    {
        dev_id      = id;
        reset       = reset_ptr;
        changed     = changed_ptr;
        lower_bound = lower_bound_ptr;
        ref         = ref_ptr;
        occupancy   = occupancy_ptr;
        distributed = distributed_ptr;
    }

    void sendPacket(uint8_t dest, uint8_t type)
    {
        byte packet[HEADER_SIZE];
        packet[0] = *dev_id;
        packet[1] = type;

        /*
        Serial.print("[I2C] Sending ");
        Serial.print(type);
        Serial.print(" to ");
        Serial.print(dest);
        Serial.println("...");
        */

        Wire.beginTransmission(dest);
        Wire.write(packet, HEADER_SIZE);
        Wire.endTransmission();

        /*
        Serial.println("[I2C] Sent!");
        */
    }

    void readPacket(byte *id, byte *type, size_t size, byte *packet){

        size_t read = 0;
        while (Wire.available() && read < size) {
            packet[read++] = Wire.read();
        }
        if (read >= 1) {
            *id = packet[0];
            *type = packet[1];
        } else {
            *id = -1;
            *type = -1;
        }
    }

    void sendConsensus(uint8_t dest, bool start, float *d_i)
    {
        size_t size = HEADER_SIZE + DATA_CONSENSUS_SIZE;
        byte packet[size];
        packet[ID] = *dev_id;
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

        /*
        if (start)  Serial.println("[I2C] Sending initiate consensus packet");
        else        Serial.println("[I2C] Sending consensus packet");
        */
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
        packet[ID] = *dev_id;
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

    void onReceive(int bytes)
    {
        byte buffer[MAX_SIZE] = {0};
        size_t read = 0;

        while (Wire.available()) {
            buffer[read++] = Wire.read();
        }

        if (read > 0){

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
                        fb.b[i] = buffer[2 + i + *dev_id * sizeof(float)];
                    }
                    if (fb.f != ND){
                        if (!*distributed){
                            *ref = fb.f;
                        }
                        *lower_bound = fb.f;
                        *occupancy = (*lower_bound <= LOW_LUX)? false : true;
                    }
                    *changed = true;
                    break;
                case DON:
                    *distributed = true;
                    break;
                case DOF:
                    *distributed = false;
                    break;
            }
        }
    }

}
