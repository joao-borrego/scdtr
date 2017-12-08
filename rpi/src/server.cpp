/**
 * @file server.cpp
 * 
 * @brief Main RPi server application
 */

#include "server.hpp"

using boost::asio::ip::tcp;

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
