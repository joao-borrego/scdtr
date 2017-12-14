/**
 * @file constants.hpp
 * @brief Multiple board system constants
 * @author António Almeida
 * @author João Borrego
 */

#pragma once

#include "Arduino.h"

/** Positive Supply Voltage */
const float VCC   = 5.0;

/* Pinout */

/** LDR analog in pin */
const int pin_ldr = A0;
/** LED PWM out pin */
const int pin_led = 11;

/* Communication */

/** EEPROM address with device ID */
const int ID_ADDR       	= 0;
/** Number of active devices */
const int N             	= 2;
/** Master ID */
const int MASTER        	= 0;
/** Baudrate value for Serial communication */ 
const long BAUDRATE 		= 115200;
/** Size of buffer for Serial communication */
const size_t BUFFER_SIZE 	= 30;

/* System properties */

/** Value of resistance in series with the ldr in Ohm */
const float LUX_R       = 10000.0;
/** $a$ parameters of ldr's characteristics */
const float LUX_A[N]    = {-0.62995, -0.62103};
/** $b$ parameters of ldr's characteristics */
const float LUX_B[N]    = {4.8487, 4.8487};

/* Controller default parameters */

/** Proportional term coefficient */
const float K_P = 2.051;
/** Integral term coefficient */
const float K_I = 136.7462;
/** Sampling time (s) */
const float T   = 0.030;

/* System properties */

/** Aproximate maximum measure for input */
const float MAX_LUX  = 100.0;
/** Value for low illuminance setting */
const float LOW_LUX  = (1.0 / 3.0) * MAX_LUX;
/** Value for high illuminance setting */
const float HIGH_LUX = (2.0 / 3.0) * MAX_LUX;

/* States */

/** Real-time control state */
const int CONTROL       = 0;
/** Calibration state */
const int CALIBRATION   = 1;
/** Consensus state */
const int CONSENSUS   	= 2;

/** I2C information broadcast period (ms) */
#define STATUS_DELAY 500

/* Other */

/** Not defined */
const int ND = -2;