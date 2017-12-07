/**
 * @file TCPServer.hpp
 */

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

#include "TCPSession.hpp"
#include "debug.hpp"
#include "constants.hpp"

/**
 * @brief      Class for tcp server.
 */
class TCPServer
{

private:

    /** TCP session acceptor */
    tcp::acceptor acceptor_;

public:

    /**
     * @brief      Constructor
     *
     * @param      io_service  The i/o service
     * @param      port        The TCP port for incoming connections
     */
    TCPServer(boost::asio::io_service & io_service, unsigned short port)
        : acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
    {
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
