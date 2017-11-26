/** 
* @file calibration.hpp
* @brief Calibration utility headers
* @author João Borrego
* @author António Almeida
*/

#pragma once

#include <Arduino.h>
#include <Wire.h>

#include "utils.hpp"
#include "constants.hpp"

// convert float to byte array  source: http://mbed.org/forum/helloworld/topic/2053/
typedef union float2bytes_t{    
    float f; 
    byte b[sizeof(float)]; 
}; 

namespace Calibration{

	/**
	 * @brief      Execute calibration and determine K matrix
	 *
	 * @param      k_matrix  The k matrix
	 * @param[in]  devices   The number of devices
	 * @param[in]  id        The current device identifier
	 */
	void execute(float *k_matrix, int devices, uint8_t id);
	
	/**
	 * @brief      Slave callback function for receiving data
	 *
	 * @param[in]  bytes  The size of the message in bytes
	 */
	void onReceive(int bytes);

	/**
	 * @brief      Slave callback function for receiving a request
	 */
	void onRequest();

	/**
	 * @brief      Gets the ldr value in LUX.
	 *
	 * @return     The ldr value in LUX units.
	 */
	float getLDRValue();

	/**
	 * @brief      Slave waits until master allows it to progress
	 */
	void inline waitReady();

	/**
	 * @brief      Master allows all slaves to progress
	 */
	void inline sendReady(int destination);

	/**
	 * @brief      Reads a float from I2C bus
	 *
	 * @return     The value read
	 */
	float readFloat();

	/**
	 * @brief      Writes a float to I2X bus
	 *
	 * @param[in]  var   The variable to be written
	 */
	void writeFloat(float var);
}