/** 
 * @file constants.hpp
 * @brief Multiple board system constants
 * @author António Almeida
 * @author João Borrego
 */

/** Positive Supply Voltage */
const float VCC     = 5.0;

/** Serial communication BAUD rate */
#define BAUDRATE    115200

/** EEPROM address for device ID */
const int ID_ADDR   = 0;
/** Number of active devices */
const int N         = 2;


/** $a$ parameters of ldr's characteristics */
const float LUX_A[N]  = {-0.62995, -0.62995};
/** $b$ parameters of ldr's characteristics */
const float LUX_B[N]  = {4.8487, 4.8487};

/** K parameter matrix */
const float K[N*N]    = {0,0,0,0};

// DEBUG
#define STATUS_DELAY 100