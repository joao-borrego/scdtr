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
#include <thread>

#include "System.hpp"
#include "TCPServer.hpp"

void tcpServer();

void i2c();

#endif