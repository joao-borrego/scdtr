/**
 * @file server.cpp
 * 
 * @brief Main RPi server application
 */

#include "server.hpp"

using boost::asio::ip::tcp;

System::ptr system_;

int main()
{
    system_ = System::ptr(new System(N, T_S, "/dev/ttyACM0", "/tmp/myfifo"));
    
    std::thread t1(i2c);
    std::thread t2(tcpServer);

    t1.join();
    t2.join();

    return 0;
}

void i2c()
{
    system_->runI2C();
}

void tcpServer()
{
    try
    {
        boost::asio::io_service io;
        TCPServer server(io, PORT, system_);
        io.run();
    }
    catch (std::exception & e)
    {
        std::cerr << e.what() << std::endl;
    }
}
