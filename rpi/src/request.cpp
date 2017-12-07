/**
 * @file request.cpp
 */

#include "request.hpp"

void parseRequest(const std::string & request, std::string & response)
{
    std::istringstream iss(request);
    std::string type, cmd, arg;
    bool total = false;

    try
    {
        iss >> type >> cmd >> arg;
    }
    catch (std::exception & e)
    {

    }
    
    if (!type.empty())
    {
        if (type == RESET)
        {
            // TODO - Command
            response = "ack";
        }
        else
        {
            if (cmd.size() == 1)
            {
                if (type == GET)
                {
                    if (arg.size() == 1 && arg[0] == TOTAL)
                    {
                        total = true;
                    }
                    else
                    {
                        try
                        {
                            int i = std::stoi(arg);
                        }
                        catch (std::exception e)
                        {
                            response = "Invalid command";
                            return;
                        }
                    }

                    char param = cmd[0];

                    switch (param)
                    {
                        case LUX:
                            response = "l";
                            break;
                        case DUTY_CYCLE:
                            response = "d";
                            break;
                        default:
                            response = "Invalid command";
                    }
                }
                else if (type == SET)
                {
                    try
                    {
                        int i = std::stoi(cmd);
                        bool val = std::stoi(cmd);
                        // TODO - Command
                        response = "ack";
                    }
                    catch (std::exception e)
                    {
                        response = "Invalid command";
                    }
                }
            }
            else
            {
                response = "Invalid command";
            }
        }
    }
    else
    {
        response = "Invalid command";
    }
}
