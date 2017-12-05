/**
 * @file tcp_server.cpp
 */

#include "tcp_server.hpp"

int main()
{
	try
	{
		boost::asio::io_service io;
		TCPServer server(io);
		io.run();
	}
	catch (std::exception & e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}