/** 
 * @file constants.hpp
 * @brief Multiple board system constants
 * @author António Almeida
 * @author João Borrego
 */

#pragma once

/** Positive Supply Voltage */
const float VCC     = 5.0;

/* Pinout */

/** LDR analog in pin */
const int pin_ldr = A0;
/** LED PWM out pin */
const int pin_led = 11;

/* Communication */

/** Serial communication BAUD rate */
#define BAUDRATE    115200

/** EEPROM address for device ID */
const int ID_ADDR       = 0;
/** Number of active devices */
const int N             = 2;
/** Master ID */
const int MASTER        = 0;

/* System properties */

/** Value of resistance in series with the ldr in Ohm */
const float LUX_R = 10000.0;
/** $a$ parameters of ldr's characteristics */
const float LUX_A[N]  = {-0.62995, -0.62453};
/** $b$ parameters of ldr's characteristics */
const float LUX_B[N]  = {4.8487, 4.9487};

// DEBUG
#define STATUS_DELAY 1000

/** K parameter matrix */
const float K[N*N]    = {0,0,0,0};