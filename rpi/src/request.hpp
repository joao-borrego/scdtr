/**
 * @file request.hpp
 */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <sstream>
#include <ctime>

#include "debug.hpp"
#include "constants.hpp"
#include "System.hpp"

/* Request types */

/** Request to obtain a certain parameter */
#define GET             "g"
/** Request to modify a certain parameter */
#define SET             "s"
/** Request to reset the system */
#define RESET           "r"
/** Get buffer with last minute information on a given variable */
#define LAST_MINUTE     "b"
/** Start "real-time" stream of given variable */
#define START_STREAM    "c"
/** Stop "real-time" stream of given variable */
#define STOP_STREAM     "d"

/* Get requests */

/** Get current measured illuminance at desk */
#define LUX             'l' // <i>
/** Get current duty cycle at luminaire */
#define DUTY_CYCLE      'd' // <i>
/** Get current occupancy state at desk */
#define OCCUPANCY       'o' // <i>
/** Get current illuminance lower bound at desk */
#define LUX_LOWER       'L' // <i>
/** Get current external illuminance at desk  */
#define LUX_EXTERNAL    'O' // <i>
/** Get current illuminance control reference at desk */
#define LUX_REF         'r' // <i>
/** Get instantaneous power consumption at desk or total */
#define POWER           'p' // <i> or T
/** Get accumulated energy consumption at desk or total */
#define ENERGY          'e' // <i> or T
/** Get accumulated comfort error at desk or total */
#define COMFORT_ERR     'c' // <i> or T
/** Get accumulated comfort varaince at desk or total */
#define COMFORT_VAR     'v' // <i> or T

/** Total modifier parameter */
#define TOTAL           'T'

/* Responses */

/** Command acknowledged and executed */
#define ACK             "ack"
/** Invalid request */
#define INVALID         "Invalid request!"

/* Functions */

//TODO
void parseRequest(
    System::ptr system,
    std::vector< bool> & flags,
    const std::string & request,
    std::string & response);

//TODO
void streamUpdate(
    System::ptr system,
    std::vector< std::time_t > & timestamp,
    const std::vector< bool> & flags,
    std::string & response);

#endif