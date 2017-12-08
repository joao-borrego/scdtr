/**
 * @file  System.cpp
 * 
 * @brief System main class
 */

#include "System.hpp"

void System::start()
{
    debugPrintTrace("System initialised.");
}

void System::readData()
{

}

void System::insertEntry(
    size_t id,
    std::time_t timestamp,
    float lux,
    int duty_cycle,
    float lux_reference)
{
    Entry new_entry = Entry(timestamp, lux, duty_cycle, lux_reference);
    try
    {
        entries_.at(id).push_back(std::move(new_entry));
    }
    catch (const std::out_of_range & e)
    {
        errPrintTrace(e.what());
    }
}

float System::getLux(size_t id)
{
    try
    {
        return (entries_.at(id).empty())?
            -1 : entries_.at(id).back().lux;
    }
    catch (const std::out_of_range & e)
    {
        errPrintTrace(e.what());
        return -1;
    }
}

int System::getDutyCycle(size_t id)
{
    try
    {
        return (entries_.at(id).empty())?
            -1 : entries_.at(id).back().duty_cycle;
    }
    catch (const std::out_of_range & e)
    {
        errPrintTrace(e.what());
        return -1;
    }
}

bool System::getOccupancy(size_t id)
{
    try
    {
        return occupancy_.at(id);
    }
    catch (const std::out_of_range & e)
    {
        errPrintTrace(e.what());
        return false;
    }
}

float System::getLuxLowerBound(size_t id)
{
    try
    {
        return lux_lower_bound_.at(id);
    }
    catch (const std::out_of_range & e)
    {
        errPrintTrace(e.what());
        return -1;
    }
}

float System::getLuxExternal(size_t id)
{
    try
    {
        return lux_external_.at(id);
    }
    catch (const std::out_of_range & e)
    {
        errPrintTrace(e.what());
        return -1;
    }
}

float System::getLuxReference(size_t id)
{
    try
    {
        return (entries_.at(id).empty())?
            -1 : entries_.at(id).back().lux_reference;
    }
    catch (const std::out_of_range & e)
    {
        errPrintTrace(e.what());
        return -1;
    }
}

float System::getPower(size_t id, bool total)
{
    float power_i, power = 0.0;
    
    if (!total)
    {
        power = getDutyCycle(id); // * 1.0 W
    }
    else
    {
        for (int i = 0; i < nodes_; i++)
        {
            power_i = getDutyCycle(id);
            power += power_i;
            if (power_i == -1) return -1;
        }
    }
    return power;
}

float System::energyNode(size_t id)
{
    float d_prev, t_prev, t_cur, energy = 0.0;
    int length = entries_.at(id).size();
    for (int i = 1; i < length; i++)
    {
        d_prev = entries_.at(id).at(i - 1).duty_cycle;
        t_prev = entries_.at(id).at(i - 1).timestamp;
        t_cur  = entries_.at(id).at(i).timestamp;
        energy += d_prev * (t_prev - t_cur);
    }
    return energy;
}

float System::getEnergy(size_t id, bool total)
{
    float energy = 0.0;
    try
    {
        if (!total)
        {
            energy = energyNode(id);
        }
        else
        {
            for (int i = 0; i < nodes_; i++)
            {
                energy += energyNode(i);
            }
        }
        return energy;
    }
    catch (const std::out_of_range & e)
    {
        errPrintTrace(e.what());
        return -1;
    }
}

float System::comfortErrorNode(size_t id)
{
    float lux_ref, lux, comfort_error = 0.0;
    int length = entries_.at(id).size();
    for (int i = 0; i < length; i++)
    {
        lux_ref = entries_.at(id).at(i).lux_reference;
        lux = entries_.at(id).at(i).lux;
        comfort_error += std::max(lux_ref - lux, 0.0f);
    }
    return comfort_error / nodes_;
}

float System::getComfortError(size_t id, bool total)
{
    float comfort_error = 0.0;
    
    try
    {
        if (!total)
        {
            comfort_error = comfortErrorNode(id);
        }
        else
        {
            for (int i = 0; i < nodes_; i++)
            {
                comfort_error += comfortErrorNode(i);
            }
        }
        return comfort_error;
    }
    catch (const std::out_of_range & e)
    {
        errPrintTrace(e.what());
        return -1;
    }
}

float System::comfortVarianceNode(size_t id)
{
    float lux, lux_1, lux_2, comfort_variance = 0.0;
    int length = entries_.at(id).size();
    for (int i = 2; i < length; i++)
    {
        lux = entries_.at(id).at(i).lux_reference;
        lux_1 = entries_.at(id).at(i-1).lux_reference;
        lux_2 = entries_.at(id).at(i-2).lux_reference;
        
        comfort_variance += (std::abs(lux - 2*lux_1 + lux_2)) / 
            (std::pow(sample_period_,2));
    }
    return comfort_variance / nodes_;
}

float System::getComfortVariance(size_t id, bool total)
{
    float comfort_variance = 0.0;
    
    try
    {
        if (!total)
        {
            comfort_variance = comfortVarianceNode(id);
        }
        else
        {
            for (int i = 0; i < nodes_; i++)
            {
                comfort_variance += comfortVarianceNode(i);
            }
        }
        return comfort_variance;
    }
    catch (const std::out_of_range & e)
    {
        errPrintTrace(e.what());
        return -1;
    }
}

void System::reset()
{
    // TODO
}

void setOccupancy(size_t id, bool occupancy)
{
    // TODO
}