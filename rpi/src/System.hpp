/**
 * @file  System.hpp
 * 
 * @brief System main class headers 
 */

#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <string>

class System
{

private:

    size_t nodes_;

public:

    System(size_t nodes)
    {

    }

    void readData();

    /* Get */

    float getLux(size_t id);

    int getDutyCycle(size_t id);

    bool getOccupancy(size_t id);

    float getLuxLowerBound(size_t id);

    float getLuxExternal(size_t id);

    float getLuxReference(size_t id);

    float getPower(size_t id, bool total);

    float getEnergy(size_t id, bool total);

    float getComfortError(size_t id, bool total);

    float getComfortVariance(size_t id, bool total);

    /* Set */

    void reset();

    void setOccupancy(size_t id, bool occupancy);

};

#endif