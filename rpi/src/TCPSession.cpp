/**
 * @file TCPSession.cpp
 */

#include "TCPSession.hpp"

void TCPSession::start()
{
    // Start the receiver actor and recv send loop
    startRead();
    // Start the timer
    timer_.expires_from_now(boost::posix_time::seconds(1));
    timer_.async_wait(boost::bind(& TCPSession::handleTimer, this,
        boost::asio::placeholders::error));
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

        if (bytes_transferred > 1)
        {
            debugPrintTrace("Received " << bytes_transferred << " bytes: " << recv_buffer_);
        }

        size_t length = 0;
        char *request_str = std::strtok(recv_buffer_, DELIMETER_STR);
        
        memset(send_buffer_, '\0', SEND_BUFFER);
        
        if (request_str)
        {
            std::string response;
            std::string request(request_str);

            parseRequest(system_, last_update_, flags_, request, response);

            length = (response.size() < SEND_BUFFER - 1)? response.size() : SEND_BUFFER - 1;
            strncpy(send_buffer_, response.c_str(), length);
            send_buffer_[length + 1] = MSG_DELIMETER;
        }
        else
        {
            // Ignore non-terminated or empty messages (e.g. heartbeat)
            send_buffer_[length] = MSG_DELIMETER;
        }

        if (length > 0)
        {
            debugPrintTrace("Sending: " << send_buffer_);
        } 

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
        if (error == boost::asio::error::eof)
            debugPrintTrace("Connection closed");
        else
            errPrintTrace(error.message());
        delete this;
    }
}

void TCPSession::startStreamWrite()
{
    boost::asio::async_write(socket_, boost::asio::buffer(stream_buffer_, SEND_BUFFER),
        boost::bind(& TCPSession::handleStreamWrite, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void TCPSession::handleStreamWrite(const boost::system::error_code & error,
    size_t bytes_transferred)
{
    if (error)
    {
        if (error == boost::asio::error::eof)
            debugPrintTrace("Connection closed");
        else
            errPrintTrace(error.message());
        delete this;
    }
}

void TCPSession::handleTimer(const boost::system::error_code & error)
{
    
    if (!error)
    {
        // Obtain stream string
        std::string response;
        streamUpdate(system_, last_update_, flags_, response); 
        
        if (!response.empty())
        {
            memset(stream_buffer_, '\0', SEND_BUFFER);
            size_t length = (response.size() < SEND_BUFFER - 1)? response.size() : SEND_BUFFER - 1;
            strncpy(stream_buffer_, response.c_str(), length);
            stream_buffer_[length + 1] = MSG_DELIMETER;
            startStreamWrite();
        }

        // Reschedule the timer
        timer_.expires_at(timer_.expires_at() + boost::posix_time::seconds(1));
        // Post the timer event
        timer_.async_wait(boost::bind(& TCPSession::handleTimer, this,
            boost::asio::placeholders::error));
    }
}