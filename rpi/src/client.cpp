/**
 * @file  client.cpp
 * 
 * @brief Synchronous TCP client 
 */

#include "client.hpp"

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: " << argv[0] << " <host> <port>\n";
            return 1;
        }

        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(tcp::v4(), argv[1], argv[2]);
        tcp::resolver::iterator iterator = resolver.resolve(query);

        tcp::socket s(io_service);
        boost::asio::connect(s, iterator);

        for(;;)
        {
            std::cout << "~ ";
            char request[SEND_BUFFER] = {'\0'};
            std::cin.getline(request, SEND_BUFFER);
            size_t request_length = std::strlen(request);
            
            if (request_length > 0)
            {
                boost::asio::write(s, boost::asio::buffer(request, request_length));
    
                char reply[PACKET_SIZE] = {'\0'};
                size_t reply_length = boost::asio::read(s,
                    boost::asio::buffer(reply, PACKET_SIZE));
                std::cout << reply << std::endl;
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
