/**
 * @file    rpi/src/server.cpp
 * 
 * @brief   Main RPi server application implementation
 * 
 * @author  João Borrego
 */

#include "server.hpp"

using boost::asio::ip::tcp;

/** Global system shared pointer */
System::ptr system_;

/**
 * @brief      Server main application.
 *
 * @param[in]  argc  The argc
 * @param      argv  The argv
 *
 * @return     0 on success, EXIT_FAILURE otherwise.
 */
int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        std::cout << "Usage:\t" << argv[0] << " <Serial> <I2C>" << std::endl;
        std::cout << " e.g.:\t" << argv[0] << " /dev/tty/ACM0   /tmp/i2c" << std::endl;
           
        exit(EXIT_FAILURE);
    }

    system_ = System::ptr(new System(NODES, T_S, argv[1], argv[2]));
    
    std::thread t1(i2c);
    std::thread t2(tcpServer);
    std::thread t3(serial);

    t1.join();
    t2.join();
    t3.join();

    return 0;
}

void i2c()
{
    system_->runI2C();
}

void serial()
{
    system_->runSerial();
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
