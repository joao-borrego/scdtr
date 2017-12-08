/**
 * @file server.hpp
 * 
 * @brief Main RPi server application headers
 */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "System.hpp"
#include "TCPServer.hpp"

/**
 * @brief      Runs TCP server.
 *
 * @param[in]  sys   The system
 */
void tcpServer(System::ptr & sys);

#endif