/**
 * @file TCPSession.hpp
 */


#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

#include "debug.hpp"
#include "constants.hpp"

/** Deadline for pending read (seconds) */
#define EXPIRE_READ         300
/** Size of receive buffer */
#define RECV_BUFFER         1024
/** Size of send buffer */
#define SEND_BUFFER         1024

/**
 * @brief      Class for tcp session.
 */
class TCPSession
{

private:

    /** TCP Socket */
    tcp::socket socket_;
    /** Request buffer */
    char recv_buffer_[RECV_BUFFER];
    /** Response buffer */
    char send_buffer_[SEND_BUFFER];

public:

    /**
     * @brief      Constructor
     *
     * @param      io_service  The i/o service
     */
    TCPSession(boost::asio::io_service & io_service)
        :   socket_(io_service){}

    /**
     * @brief      Obtains TCP socket.
     *
     * @return     TCP socket
     */
    tcp::socket & socket()
    {
        return socket_;
    }

    /**
     * @brief      Starts the session.
     */
    void start();

private:

    /**
     * @brief      Starts a read.
     */
    void startRead();

    /**
     * @brief      Handles a read.
     *
     * @param[in]  error  The error code
     */
    void handleRead(const boost::system::error_code & error,
        size_t bytes_transferred);

    /**
     * @brief      Starts a write.
     */
    void startWrite();

    /**
     * @brief      Answers a pending request.
     *
     * @param[in]  error           The error code
     * @param[in]  bytes_transferred  The bytes transferred
     */
    void handleWrite(const boost::system::error_code & error,
        size_t bytes_transferred);
};
