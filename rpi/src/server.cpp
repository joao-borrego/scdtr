/**
 * @file server.cpp
 * 
 * @brief Main RPi server application
 */

#include "server.hpp"

using boost::asio::ip::tcp;

int main()
{
    System::ptr sys(new System(N, T_S, "/dev/ttyACM0"));
    tcpServer(sys);
    return 0;
}

void tcpServer(System::ptr & sys)
{
    try
    {
        boost::asio::io_service io;
        TCPServer server(io, PORT, sys);
        io.run();
    }
    catch (std::exception & e)
    {
        std::cerr << e.what() << std::endl;
    }
}
