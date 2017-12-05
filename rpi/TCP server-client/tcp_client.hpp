/**
 * @file tcp_client.hpp
 */

#include <unistd.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::deadline_timer;
using boost::asio::ip::tcp;

#include "debug.hpp"
#include "constants.hpp"

class TCPClient
{

private:

    /** Whether client is stopped */
    bool stopped_;
    /** TCP socket */
    tcp::socket socket_;
    /** TODO */
    boost::asio::streambuf input_buffer_;
    /** TODO */
    deadline_timer deadline_;
    /** TODO */
    deadline_timer heartbeat_timer_;
    /** TODO */
    boost::asio::posix::stream_descriptor input_;
    /** TODO */
    boost::asio::streambuf console_buffer_;
    /** TODO */
    enum {max_length = 1024};
    /** TODO */
    char send_buffer_[max_length];

public:

    TCPClient(boost::asio::io_service & io_service)
        :   stopped_(false),
            socket_(io_service),
            deadline_(io_service),
            heartbeat_timer_(io_service),
            input_(io_service, ::dup(STDIN_FILENO)),
            console_buffer_(CONSOLE_BUFFER) {}

    /**
     * @brief      Starts the client.
     * 
     * Initiates a TCP connection proccess.
     * The endpoint iterator is obtained through a tcp::resolver
     *
     * @param[in]  endpoint_iter  The endpoint iterator
     */
    void start(tcp::resolver::iterator endpoint_iter);

    /**
     * @brief      Terminates the client.
     * 
     * Terminates all the actors in order to shut down a connection
     * 
     */
    void stop();

private:

    /**
     * @brief      Starts a TCP connection
     *
     * @param[in]  endpoint_iter  The endpoint iterator
     */
    void startConnect(tcp::resolver::iterator endpoint_iter);

    /**
     * @brief      Handles a new TCP connection
     *
     * @param[in]  err_code       The error code
     * @param[in]  endpoint_iter  The endpoint iterator
     */
    void handleConnect(const boost::system::error_code & err_code,
        tcp::resolver::iterator endpoint_iter);

    /**
     * @brief      Starts a read.
     */
    void startRead();

    /**
     * @brief      Handles a read.
     *
     * @param[in]  err_code  The error code
     */
    void handleRead(const boost::system::error_code & err_code);

    /**
     * @brief      Starts a write.
     */
    void startWrite();

    /**
     * @brief      Handles a write.
     *
     * @param[in]  err_code  The error code
     */
    void handleWrite(const boost::system::error_code & err_code);

    /**
     * @brief      Checks if deadline was reached
     */
    void checkDeadline();

    /**
     * @brief      Starts a console read.
     */
    void startReadConsole();

    /**
     * @brief      Handles a console read.
     *
     * @param[in]  err_code  The error code
     * @param[in]  length    The length
     */
    void handleReadConsole(const boost::system::error_code & err_code,
        size_t length);

    /**
     * @brief      Handles a send.
     *
     * @param[in]  err_code  The error code
     * @param[in]  length    The length
     */
    void handleSend(const boost::system::error_code & err_code,
        size_t length);
};