/**
 * @file request.cpp
 */

#include "request.hpp"
#include "System.hpp"

void parseRequest(
    System::ptr system,
    const std::string & request,
    std::string & response)
{
    std::istringstream iss(request);
    std::string type, cmd, arg;
    int id = -1;
    bool total = false;

    if (!system)
    {
        response = INVALID;
        return;
    }

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
            response = ACK;
        }
        else
        {
            if (cmd.size() == 1)
            {
                if (type == GET)
                {
                    char param = cmd[0];
                    
                    if (arg.size() == 1 && arg[0] == TOTAL)
                    {
                        if (param == POWER || param == ENERGY ||
                            param == COMFORT_ERR || param == COMFORT_VAR)
                        {
                            total = true;
                        }
                        else
                        {
                            response = INVALID;
                            return;    
                        }
                    }
                    else
                    {
                        try
                        {
                            id = std::stoi(arg);
                            if (id < 0 || id >= system->getNodes()) throw std::exception();
                        }
                        catch (std::exception e)
                        {
                            response = INVALID;
                            return;
                        }
                    }

                    // Prevent cross initialisation errors in switch
                    float value_f = -1;
                    int value_i = -1;
                    bool value_b = 0;

                    switch (param)
                    {
                        case LUX:
                            value_f = system->getLux(id);
                            response = std::string(1,LUX) + " " 
                                + std::to_string(value_f);
                            break;
                        case DUTY_CYCLE:
                            value_i = system->getDutyCycle(id);
                            response = std::string(1, DUTY_CYCLE) + " "
                                + std::to_string(value_i);
                            break;
                        case OCCUPANCY:
                            value_b = system->getOccupancy(id);
                            response = std::string(1, OCCUPANCY) + " "
                                + std::to_string(value_b);
                            break;
                        case LUX_LOWER:
                            value_f = system->getLuxLowerBound(id);
                            response = std::string(1, LUX_LOWER) + " "
                                + std::to_string(value_f);
                            break;
                        case LUX_EXTERNAL:
                            value_f = system->getLuxExternal(id);
                            response = std::string(1, LUX_EXTERNAL) + " "
                                + std::to_string(value_f);
                            break;
                        case LUX_REF:
                            value_f = system->getLuxReference(id);
                            response =  std::string(1, LUX_REF) + " "
                                + std::to_string(value_f);
                            break;
                        case POWER:
                            value_f = system->getPower(id, total);
                            response =  std::string(1, POWER) + " " + 
                                ((id == -1 && total)? std::string(1, TOTAL) : std::to_string(id))
                                + " " + std::to_string(value_f);
                            break;
                        case ENERGY:
                            value_f = system->getEnergy(id, total);
                            response =  std::string(1, ENERGY) + " " + 
                                ((id == -1 && total)? std::string(1, TOTAL) : std::to_string(id))
                                + " " + std::to_string(value_f);
                            break;
                        case COMFORT_ERR:
                            value_f = system->getComfortError(id, total);
                            response =  std::string(1, COMFORT_ERR) + " " + 
                                ((id == -1 && total)? std::string(1, TOTAL) : std::to_string(id))
                                + " " + std::to_string(value_f);
                            break;
                        case COMFORT_VAR:
                            value_f = system->getComfortVariance(id, total);
                            response =  std::string(1, COMFORT_VAR) + " " + 
                                ((id == -1 && total)? std::string(1, TOTAL) : std::to_string(id))
                                + " " + std::to_string(value_f);
                            break;
                        default:
                            response = INVALID;
                    }
                }
                else if (type == SET)
                {
                    try
                    {
                        int i = std::stoi(cmd);
                        bool val = std::stoi(cmd);
                        // TODO - Command
                        response = ACK;
                    }
                    catch (std::exception e)
                    {
                        response = INVALID;
                    }
                }
            }
            else
            {
                response = INVALID;
            }
        }
    }
    else
    {
        response = INVALID;
    }
}
