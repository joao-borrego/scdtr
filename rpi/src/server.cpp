/**
 * @file server.cpp
 * 
 * @brief Main RPi server application
 */

#include "server.hpp"

using boost::asio::ip::tcp;

int main()
{
    System sys(N, T_S);
    tcpServer();
    return 0;
}

void tcpServer()
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
}
