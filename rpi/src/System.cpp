/**
 * @file  System.cpp
 * 
 * @brief System main class
 */

#include "System.hpp"

void System::readData()
{

}

float System::getLux(size_t id)
{
    return 0.0;
}

int System::getDutyCycle(size_t id)
{
    return 0;
}

bool System::getOccupancy(size_t id)
{
    return false;
}

float System::getLuxLowerBound(size_t id)
{
    return 0.0;
}

float System::getLuxExternal(size_t id)
{
    return 0.0;
}

float System::getLuxReference(size_t id)
{
    return 0.0;
}

float System::getPower(size_t id, bool total)
{
    float power = 0.0;
    
    if (!total)
    {
        power = 0.0;
    }
    else
    {
        for (int i = 0; i < nodes_; i++)
        {
            power += 0.0;
        }
    }
    return power;
}

float System::getEnergy(size_t id, bool total)
{
    float energy = 0.0;
    
    if (!total)
    {
        energy = 0.0;
    }
    else
    {
        for (int i = 0; i < nodes_; i++)
        {
            energy += 0.0;
        }
    }
    return energy;
}

float System::getComfortError(size_t id, bool total)
{
    float comfort_error = 0.0;
    
    if (!total)
    {
        comfort_error = 0.0;
    }
    else
    {
        for (int i = 0; i < nodes_; i++)
        {
            comfort_error += 0.0;
        }
    }
    return comfort_error;
}

float System::getComfortVariance(size_t id, bool total)
{
    float comfort_variance = 0.0;
    
    if (!total)
    {
        comfort_variance = 0.0;
    }
    else
    {
        for (int i = 0; i < nodes_; i++)
        {
            comfort_variance += 0.0;
        }
    }
    return comfort_variance;
}

void System::reset()
{

}

void setOccupancy(size_t id, bool occupancy)
{
    
}