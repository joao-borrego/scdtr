/**
 * @file TCPSession.hpp
 */

#include <iostream>
#include <ctime>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

#include "debug.hpp"
#include "constants.hpp"
#include "System.hpp"
#include "request.hpp"

/**
 * @brief      Class for TCP session.
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
    /** System pointer */
    System::ptr system_;

    /* "Real-time" data stream */

    // TODO - mutex?
    /** Timestamp of last update */
    std::vector< std::time_t > last_update_;
    /** Boolean flags */
    std::vector< bool > flags_;
    /** Timer */
    boost::asio::deadline_timer timer_;
    /** Stream buffer */
    char stream_buffer_[SEND_BUFFER];

public:

    /**
     * @brief      Constructor
     *
     * @param      io_service  The i/o service
     */
    TCPSession(boost::asio::io_service & io_service, System::ptr system)
        :   socket_(io_service),
            flags_(STREAM_FLAGS),
            last_update_(system->getNodes()),
            timer_(io_service)
    {
        system_ = system;
    }

    ~TCPSession()
    {
        timer_.cancel();
    }

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
     * @brief      Handles a write response.
     *
     * @param[in]  error                The error code
     * @param[in]  bytes_transferred    The bytes transferred
     */
    void handleWrite(const boost::system::error_code & error,
        size_t bytes_transferred);

    /**
     * @brief      Starts a stream write.
     */
    void startStreamWrite();

    /**
     * @brief      Handles a write stream.
     *
     * @param[in]  error              The error code
     * @param[in]  bytes_transferred  The bytes transferred
     */
    void handleStreamWrite(const boost::system::error_code & error,
        size_t bytes_transferred);

    /**
     * @brief      Handles a periodic timer event.
     */
    void handleTimer(const boost::system::error_code & error);

};
