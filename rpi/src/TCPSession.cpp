/**
 * @file TCPSession.cpp
 */

#include "TCPSession.hpp"


void TCPSession::start()
{
    // Start the receiver actor and recv send loop
    startRead();
    // Start the timer
    timer_.async_wait(boost::bind(& TCPSession::handleTimer, this));
}

void TCPSession::startRead()
{
    memset(recv_buffer_, '\0', RECV_BUFFER);
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

        size_t length = 0;
        char *request_str = std::strtok(recv_buffer_, DELIMETER_STR);
        
        if (request_str)
        {
            std::string response;
            std::string request(request_str);

            parseRequest(system_, flags_, request, response);
            strcpy(send_buffer_, response.c_str());
            length = response.size();
        }
        else
        {
            // Ignore non-terminated or empty messages (e.g. heartbeat)
            debugPrintTrace("Empty message");
        }

        send_buffer_[length] = '\0';
        startWrite();
    }
    else
    {
        if (error == boost::asio::error::eof)
            debugPrintTrace("Connection closed");
        else
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

void TCPSession::handleTimer()
{
    debugPrintTrace("[Timer]");

    // Reschedule the timer
    timer_.expires_at(timer_.expires_at() + boost::posix_time::seconds(1));
    // Post the timer event
    timer_.async_wait(boost::bind(& TCPSession::handleTimer, this));
}