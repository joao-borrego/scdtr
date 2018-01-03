/**
 * @file    rpi/src/test_client.cpp
 * 
 * @brief   Test client application
 * 
 * @author  João Borrego
 */

#include <iostream>
#include <boost/asio.hpp>

#include <vector>
#include <chrono>
#include <thread>

namespace asio = boost::asio;

/**
 * @brief      Class for instruction.
 */
class Instruction {

public:

    std::string command;
    int delay;

    /**
     * @brief      Constructs an instruction
     *
     * @param[in]  cmd   The command
     * @param[in]  t     The delay time
     */
    Instruction(std::string cmd, int t):
        command(cmd), delay(t) { }
};

/**
 * @brief      Test client main application
 *
 * @param[in]  argc  The argc
 * @param      argv  The argv
 *
 * @return     0 on success
 */
int main(int argc, char** argv) {

    // Connect to server
    asio::io_service io_service;
    asio::ip::tcp::socket socket(io_service);
    socket.connect( asio::ip::tcp::endpoint( asio::ip::address::from_string(argv[1]), atoi(argv[2]) ) );
    std::cout << "Connected to server." << std::endl;

    // Script
    std::vector<Instruction> script;

    script.emplace_back(std::string("r"),       4000);  // Reset system
    script.emplace_back(std::string("D"),       6000);  // Isolated controller
    script.emplace_back(std::string("s 0 1"),   10000); // Set occupancy to 1 in node 0
    script.emplace_back(std::string("s 1 1"),   10000); // Set occupancy to 1 in node 1
    script.emplace_back(std::string("s 0 0"),   10000); // Set occupancy to 1 in node 0
    script.emplace_back(std::string("s 1 0"),   10000); // Set occupancy to 1 in node 0
    script.emplace_back(std::string("S"),       0);     // Save data to file
        
    // Run script
    boost::system::error_code error;
    for (auto i : script){
        
        asio::write( socket, asio::buffer(i.command), error );

        if( error ) {
            std::cout << "Send failed: " << error.message() << std::endl;
        }
        else {
            std::cout << "Sent " << i.command << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(i.delay));
    }

    std::cout << "Success! Exiting..." << std::endl;
    return 0;
}