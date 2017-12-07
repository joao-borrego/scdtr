/**
 * @file TCPServer.cpp
 */

#include "TCPServer.hpp"

void TCPServer::startAccept()
{
    TCPSession* new_session = new TCPSession(acceptor_.get_io_service());

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

int main()
{
    try
    {
        boost::asio::io_service io;
        TCPServer server(io, PORT);
        io.run();
    }
    catch (std::exception & e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
