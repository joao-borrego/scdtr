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

#ifndef COMMUNICATION_HPP
#define COMMUNICATION_HPP

#include "constants.hpp"

/** Packet maximum size (bytes) - imposed by Wire.h */
const size_t MAX_SIZE               = 32;
/** Packet header size (bytes) */
const size_t HEADER_SIZE            = sizeof(uint8_t) * 2;
/** Consensus data size (bytes) */
const size_t DATA_CONSENSUS_SIZE    = sizeof(float) * NODES;
/** Information data size (bytes) */
const size_t DATA_INFO_SIZE         = sizeof(float) * 3 + sizeof(uint8_t) * 2;

/* Packet types */

/** SYNchronise */
const uint8_t SYN   = 0x00;
/** ACKnowledgement */
const uint8_t ACK   = 0x01;
/** CONsensus */
const uint8_t CON   = 0x02;
/** Initiate COnsensus */
const uint8_t ICO   = 0x04;
/** INFo */
const uint8_t INF   = 0x05;
/** RESet */
const uint8_t RES   = 0x0F;

/* Packet offsets */
const size_t ID     = 0;
const size_t TYPE   = 1;

const int

namespace Communication
{
    /**
     * @brief Converts float to byte array and vice-versa.
     * 
     * As seen in <a href="http://mbed.org/forum/helloworld/topic/2053/">link</a>
     */
    typedef union float_to_bytes_t{    
        float f; 
        unsigned char b[sizeof(float)]; 
    } float_bytes;

}

#endif