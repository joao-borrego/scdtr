/**
 * @file tcp_server.hpp
 */

#include <iostream>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

#include "constants.hpp"

/**
 * @brief      Class for tcp connection.
 */
class TCPConnection
    : public boost::enable_shared_from_this<TCPConnection>
{

private:

    /** TCP Socket */
    tcp::socket socket_;
    /** Greeting message */
    std::string message_;

public:

    /** Type definition for TCPConnection shared pointer */
    typedef boost::shared_ptr<TCPConnection> pointer;

    static pointer create(boost::asio::io_service & io_service)
    {
        return pointer(new TCPConnection(io_service));
    }

    /**
     * @brief      Obtain TCP socket
     *
     * @return     TCP socket
     */
    tcp::socket & socket()
    {
        return socket_;
    }

    /**
     * @brief      Sends greeting message to client
     */
    void start()
    {
        message_ = "Welcome to the server!";

        boost::asio::async_write(socket_, boost::asio::buffer(message_),
            boost::bind(& TCPConnection::handleWrite,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

private:

    TCPConnection(boost::asio::io_service & io_service)
        : socket_(io_service) {}

    void handleWrite(
        const boost::system::error_code & err_code,
        size_t bytes_transferred) {}
};

/**
 * @brief      Class for tcp server.
 */
class TCPServer
{

private:

    /** TCP connection acceptor */
    tcp::acceptor acceptor_;

public:

    /**
     * @brief      Constructor
     *
     * @param      io_service  The i/o service
     */
    TCPServer(boost::asio::io_service & io_service)
        : acceptor_(io_service, tcp::endpoint(tcp::v4(), PORT))
    {
        startAccept();
    }

private:

    /**
     * @brief      Starts an accept connection
     */
    void startAccept()
    {
        TCPConnection::pointer new_connection =
            TCPConnection::create(acceptor_.get_io_service());

        acceptor_.async_accept(new_connection->socket(),
            boost::bind(& TCPServer::handleAccept, this, new_connection,
                boost::asio::placeholders::error));
    }

    /**
     * @brief      Handles and accept attempt
     */
    void handleAccept(TCPConnection::pointer new_connection,
        const boost::system::error_code & error)
    {
        if (!error){
            new_connection->start();
            startAccept();
        }
    }
};
