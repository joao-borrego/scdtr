/**
 * @file    rpi/src/TCPServer.hpp
 * 
 * @brief   TCP Server class headers
 * 
 * @author  João Borrego
 */

#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

#include "TCPSession.hpp"
#include "System.hpp"
#include "debug.hpp"
#include "constants.hpp"

/**
 * @brief      Class for TCP server.
 */
class TCPServer
{

private:

    /** TCP session acceptor */
    tcp::acceptor acceptor_;
    /** System pointer */
    System::ptr system_;

public:

    /**
     * @brief      Constructor
     *
     * @param      io_service  The i/o service
     * @param      port        The TCP port for incoming connections
     * @param[in]  system      The system
     */
    TCPServer(
        boost::asio::io_service & io_service,
        unsigned short port,
        System::ptr system)
        : acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
    {
        system_ = system;
        startAccept();
    }

private:

    /**
     * @brief      Starts an accept session
     */
    void startAccept();

    /**
     * @brief      Handles and accept attempt
     */
    void handleAccept(TCPSession *new_session,
      const boost::system::error_code & error);
};

#endif