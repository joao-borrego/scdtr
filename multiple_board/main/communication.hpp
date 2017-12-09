/**
 * @file communication.hpp
 * 
 * @brief Communication protocol headers
 * 
 * Messages can be exchanged using essentially 4 types of packets
 * 1. Header-only, e.g ACKs and RESet packets,
 * 2. Consensus,
 * 3. Info.
 * 
 * Header       = [ Id | Type ]
 * Consensus    = [ Header | d_i_1 d_i_2 ... d_i_N ]
 * Info         = [ Header | d_i | l_i | r_i | o_i ]
 */

#pragma once

#include <Arduino.h>
#include <Wire.h>

#include "constants.hpp"

/** Packet maximum size (bytes) - imposed by Wire.h */
const size_t MAX_SIZE               = 32;
/** Packet header size (bytes) */
const size_t HEADER_SIZE            = sizeof(uint8_t) * 2;
/** Consensus data size (bytes) */
const size_t DATA_CONSENSUS_SIZE    = sizeof(float) * N;
/** Information data size (bytes) */
const size_t DATA_INFO_SIZE         = sizeof(float) * 3 + sizeof(uint8_t) * 2;

/* Packet types */

/** ACKnowledgement */
const byte ACK      = 0x00;
/** CONsensus */
const byte CON      = 0x01;
/** Initiate COnsensus */
const byte ICO      = 0x02;
/** INFo */
const byte INF      = 0x03;
/** RESet */
const byte RES      = 0x0F;

/* Packet offsets */
const size_t ID     = 0;
const size_t TYPE   = 1;

namespace Communication
{
    /**
     * @brief Converts float to byte array and vice-versa.
     * 
     * As seen in <a href="http://mbed.org/forum/helloworld/topic/2053/">link</a>
     */
    typedef union float_to_bytes_t{    
        float f; 
        byte b[sizeof(float)]; 
    } float_bytes;

    /* Functions */

    /**
     * @brief      Sets the device identifier.
     *
     * @param[in]  id    The identifier
     */
    void setDeviceId(uint8_t id);

    /**
     * @brief      Sends an acknowledge packet.
     *
     * @param[in]  dest    The destination id
     */
    void sendAck(uint8_t dest);

    /**
     * @brief      Sends a consensus packet.
     *
     * @param[in]  dest  The destination id     
     * @param      d_i   The d i
     */
    void sendConsensus(uint8_t dest, float *d_i);

    /**
     * @brief      Reads a consensus packet.
     *
     * @param      buffer  The message buffer
     * @param      out     The array of read floats
     */
    void readConsensus(byte *buffer, float *out);

    /**
     * @brief      Reads to buffer.
     *
     * @param      buffer  The buffer
     *
     * @return     The amount of bytes read
     */
    size_t readToBuffer(byte *buffer);

    /**
     * @brief      Sends an information packet.
     *
     * @param[in]  dest       The destination id
     * @param[in]  lux        The illuminance
     * @param[in]  ref        The reference illuminance
     * @param[in]  ext        The external illuminance
     * @param[in]  occupancy  The occupancy
     */
    void sendInfo(uint8_t dest, float lux, float ref, float ext, bool occupancy);

    /**
     * @brief      Callback function for processing received data
     *
     * @param[in]  bytes  The bytes
     */
    void onReceive(int bytes);

}
