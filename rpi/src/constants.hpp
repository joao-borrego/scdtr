/**
 * @file    rpi/src/constants.hpp
 * 
 * @brief  	Relevant system constants
 * 
 * @author  João Borrego
 */

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

/** Number of nodes in system */
#define NODES   2
/** System sampling period (seconds) */
#define T_S 0.1

/** Server host */
#define HOST "127.0.0.1"
/** Listenning port for TCP server */
#define PORT 17000

/** Message delimiter */
#define MSG_DELIMETER '\n'
/** Message delimiter string */
#define DELIMETER_STR "\n"

/** Size of receive buffer */
#define RECV_BUFFER 1024
/** Size of send buffer */
#define SEND_BUFFER 1023
/** Size of packet */
#define PACKET_SIZE 50

/** Serial communication Baudrate */
#define SERIAL_BAUDRATE 115200

/* TCP Stream */

/** Stream period (ms) */
#define STREAM_PERIOD 300
/** Stream flags (one per shown variable) */
#define STREAM_FLAGS 2

#endif
