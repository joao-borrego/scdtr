/**
 * @file    rpi/src/TCPServer.cpp
 * 
 * @brief   TCP Server class implementation
 * 
 * @author  João Borrego
 */

#include "TCPServer.hpp"

void TCPServer::startAccept()
{
    TCPSession* new_session = new TCPSession(acceptor_.get_io_service(), system_);

    acceptor_.async_accept(new_session->socket(),
        boost::bind(& TCPServer::handleAccept, this, new_session,
            boost::asio::placeholders::error));
}

void TCPServer::handleAccept(TCPSession *new_session,
    const boost::system::error_code & error)
{  
    if (!error)
    {
        new_session->start();
        startAccept();
    }
    else
    {
        delete new_session;
    }
}