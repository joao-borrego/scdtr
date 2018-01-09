/**
 * @file    rpi/src/communication.hpp
 * 
 * @brief   I2C communication protocol headers
 * 
 * Important constants for parsing I2C messages.
 * Check protocol definition in multiple_board/main/communication.hpp.
 *
 * @author  João Borrego
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

/** ID header field offset */
const size_t ID     = 0;
/** TYPE header field offset */
const size_t TYPE   = 1;

namespace Communication
{
    /**
     * @brief Converts float to byte array and vice-versa.
     *
     * As seen in <a href="http://mbed.org/forum/helloworld/topic/2053/">link</a>
     */
    typedef union float_to_bytes_t{
        /** Float variable */
        float f;
        /** Float byte array */
        unsigned char b[sizeof(float)];
    } float_bytes;

}

#endif