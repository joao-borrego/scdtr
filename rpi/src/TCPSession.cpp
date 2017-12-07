/**
 * @file TCPSession.cpp
 */

#include "TCPSession.hpp"


void TCPSession::start()
{
    // Start the receiver actor
    startRead();
}

void TCPSession::startRead()
{
    socket_.async_read_some(boost::asio::buffer(recv_buffer_, RECV_BUFFER),
        boost::bind(& TCPSession::handleRead, this, 
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void TCPSession::handleRead(const boost::system::error_code & error,
    size_t bytes_transferred)
{
    if (!error)
    {

        debugPrintTrace("Received " << bytes_transferred << 
            " bytes: " << recv_buffer_);

        char *request = std::strtok(recv_buffer_, DELIMETER_STR);
        if (request)
        {
            std::string response("Response");
            strcpy(send_buffer_, response.c_str());
        }
        else
        {
            // Ignore non-terminated or empty messages (e.g. heartbeat)
            debugPrintTrace("Empty message");
        }

        startWrite();
    }
    else
    {
        errPrintTrace(error.message());
        delete this;
    }
}

void TCPSession::startWrite()
{
    boost::asio::async_write(socket_, boost::asio::buffer(send_buffer_, SEND_BUFFER),
        boost::bind(& TCPSession::handleWrite, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void TCPSession::handleWrite(const boost::system::error_code & error,
    size_t bytes_transferred)
{
    if (!error)
    {
        startRead();
    }
    else
    {
        errPrintTrace(error.message());
        delete this;
    }
}