/**
 * @file request.cpp
 */

#include "request.hpp"
#include "System.hpp"

void parseRequest(const std::string & request, std::string & response)
{
    std::istringstream iss(request);
    std::string type, cmd, arg;
    int id = -1;
    bool total = false;

    try
    {
        iss >> type >> cmd >> arg;
    }
    catch (std::exception & e)
    {
        errPrintTrace(e.what());
        return;
    }
    
    if (type.size() == 1)
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
                            id = std::stoi(arg);
                            if (id < 0) throw std::exception();
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
                            //float value = getLux(id);
                            response = std::string(1,LUX) + " ";
                            break;
                        case DUTY_CYCLE:
                            //int value = getDutyCycle(id);
                            response = std::string(1, DUTY_CYCLE) + " ";
                            break;
                        case OCCUPANCY:
                            //bool value = getOccupancy(id);
                            response = std::string(1, OCCUPANCY) + " ";
                            break;
                        case LUX_LOWER:
                            //float value = getLuxLowerBound(id);
                            response = std::string(1, LUX_LOWER) + " ";
                            break;
                        case LUX_EXTERNAL:
                            //float value = getLuxExternal(id);
                            response = std::string(1, LUX_EXTERNAL) + " ";
                            break;
                        case LUX_REF:
                            //float value = getLuxReference(id);
                            response =  std::string(1, LUX_REF) + " ";
                            break;
                        case POWER:
                            //float value = getPower(id, total);
                            response =  std::string(1, POWER) + " " + 
                                ((id == -1 && total)? std::string(1, TOTAL) : std::to_string(id)) +
                                " ";
                            break;
                        case ENERGY:
                            //float value = getEnergy(id, total);
                            response =  std::string(1, ENERGY) + " " + 
                                ((id == -1 && total)? std::string(1, TOTAL) : std::to_string(id)) +
                                " ";
                            break;
                        case COMFORT_ERR:
                            //float value = getComfortError(id, total);
                            response =  std::string(1, COMFORT_ERR) + " " + 
                                ((id == -1 && total)? std::string(1, TOTAL) : std::to_string(id)) +
                                " ";
                            break;
                        case COMFORT_VAR:
                            //float value = getComfortVariance(id, total);
                            response =  std::string(1, COMFORT_VAR) + " " + 
                                ((id == -1 && total)? std::string(1, TOTAL) : std::to_string(id)) +
                                " ";
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
