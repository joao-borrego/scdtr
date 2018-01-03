/**
 * @file  	rpi/src/server.hpp
 * 
 * @brief 	Main RPi server application headers
 * 
 * @author 	João Borrego
 */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <thread>

#include "System.hpp"
#include "TCPServer.hpp"

/**
 * @brief      Runs the TCP server application.
 */
void tcpServer();

/**
 * @brief      Runs the System's I2C packet listener service.
 */
void i2c();

/**
 * @brief      Launches a separate handle for Serial connections.
 */
void serial();

#endif