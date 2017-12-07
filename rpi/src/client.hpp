/**
 * @file  client.hpp
 * 
 * @brief Synchronous TCP client headers 
 */

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>

#include "constants.hpp"

/** Size of receive buffer */
#define RECV_BUFFER	1024
/** Size of send buffer */
#define SEND_BUFFER 1024