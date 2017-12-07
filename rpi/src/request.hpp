/**
 * @file request.hpp
 */

#include <iostream>
#include <sstream>

#include "debug.hpp"

/* Request types */

/** Request to obtain a certain parameter */
#define GET     "g"
/** Request to modify a certain parameter */
#define SET     "s"
/** Request to reset the system */
#define RESET   "r"

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
#define POWER_INST      'p' // <i> or T
/** Get accumulated energy consumption at desk or total */
#define ENERGY          'e' // <i> or T
/** Get accumulated comfort error at desk or total */
#define COMFORT_E       'c' // <i> or T
/** Get accumulated comfort varaince at desk or total */
#define COMFORT_V       'v' // <i> or T

/** Total modifier parameter */
#define TOTAL           'T'

/* Functions */

/**
 * @brief      Returns a response for a given request
 *
 * @param[in]  request   The request
 * @param      response  The response
 */
void parseRequest(const std::string & request, std::string & response);