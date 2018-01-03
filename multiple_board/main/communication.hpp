/**
 * @file    multiple_board/main/communication.hpp
 * 
 * @brief   I2C communication protocol headers
 *
 * Defines I2C communication protocol.
 * Implements functions to send supported packets.
 * 
 * Messages can be exchanged using essentially 4 types of packets:
 *  1. Header-only, e.g ACKs and RESet packets;
 *  2. Consensus;
 *  3. Info.
 *
 * Header       [ Id | Type ]
 *
 * Consensus    [ Header | d_i_1 d_i_2 ... d_i_N ],
 *  CON (during consensus) d_i is the solution vector for a given iteration
 *  ICO (before consessus) d_i is the lower_bound vector
 *
 * Info         [ Header | lux | dc | lb | ext | ref | occ ]
 *
 * @author  João Borrego
 * @author  António Almeida
 */

#ifndef INO_COMMUNICATION_HPP
#define INO_COMMUNICATION_HPP

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
const size_t DATA_INFO_SIZE         = sizeof(float) * 5 + sizeof(uint8_t);

/* Packet types */

/** SYNchronise */
const byte SYN      = 0x00;
/** ACKnowledgement */
const byte ACK      = 0x01;
/** CONsensus */
const byte CON      = 0x02;
/** Initiate COnsensus */
const byte ICO      = 0x04;
/** INFo */
const byte INF      = 0x05;
/** Distributed ON */
const byte DON      = 0x06;
/** Distributed OFf */
const byte DOF      = 0x07;
/** RESet */
const byte RES      = 0x0F;

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
        byte b[sizeof(float)];
    } float_bytes;

    /* Functions */

    /**
     * @brief      Sets up important variables.
     *
     * @param[in]  id               The node identifier
     * @param[in]  reset_ptr        The reset pointer
     * @param[in]  changed_ptr      The changed pointer
     * @param[in]  ref_ptr          The reference pointer
     * @param[in]  lower_bound_ptr  The lower bound pointer
     * @param[in]  occupancy_ptr    The occupancy pointer
     * @param[in]  distributed_ptr  The distributed control flag pointer
     */
    void setup(
        const uint8_t *id,
        const bool *reset_ptr,
        const bool *changed_ptr,
        const float *ref_ptr,
        const float *lower_bound_ptr,
        const bool *occupancy_ptr,
        const bool  *distributed_ptr);

    /**
     * @brief      Empty callback function to ignore I2C messages.
     *
     * @param[in]  bytes  The bytes to be read
     */
    void nop(int bytes);

    /**
     * @brief      Sends a header-only packet.
     *
     * @param[in]  dest  The destination
     * @param[in]  type  The type
     */
    void sendPacket(uint8_t dest, uint8_t type);

    /**
     * @brief      Reads a packet.
     *
     * @param      id_     The identifier
     * @param      type    The type
     * @param[in]  size    The size
     * @param      packet  The packet
     */
    void readPacket(byte *id_, byte *type, size_t size, byte *packet);

    /**
     * @brief      Sends a consensus packet
     *
     * @param[in]  dest   The destination
     * @param[in]  start  Whether consensus has started
     * @param      d_i    The solution or lower_bound arrays accordingly
     */
    void sendConsensus(uint8_t dest, bool start, float *d_i);

    /**
     * @brief      Reads a consensus packet.
     *
     * @param      buffer  The message buffer
     * @param      out     The array of read floats
     */
    void readConsensus(byte *buffer, float *out);

    /**
     * @brief      Sends an information packet
     *
     * @param[in]  dest         The destination
     * @param[in]  lux          The lux
     * @param[in]  duty_cycle   The duty cycle
     * @param[in]  lower_bound  The lower bound
     * @param[in]  ext          The external lux
     * @param[in]  ref          The reference
     * @param[in]  occupancy    The occupancy
     */
    void sendInfo(
        uint8_t dest,
        float lux,
        float duty_cycle,
        float lower_bound,
        float ext,
        float ref,
        bool occupancy);

    /**
     * @brief      Callback function for processing received data
     *
     * @param[in]  bytes  The bytes
     */
    void onReceive(int bytes);

}

#endif
