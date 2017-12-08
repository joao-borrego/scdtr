/**
 * @file  System.hpp
 * 
 * @brief System main class headers 
 */

#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <string>
#include <ctime>
#include <vector>
#include <list>
#include <algorithm>


#include "debug.hpp"

class Entry
{

public:

    /** Entry timestamp */
    std::time_t timestamp;
    /** Registered lux value */
    float lux;
    /** Registered duty cycle */
    int duty_cycle;
    /** Reference illuminance */
    float lux_reference;

    /**
     * @brief      Constructor
     *
     * @param[in]  t_s   The timestamp
     * @param[in]  l     The registered lux value
     * @param[in]  d_c   The registered duty cycle
     * @param[in]  l_r   The reference illuminance
     */
    Entry(std::time_t t_s, float l, int d_c, float l_r)
        : timestamp(t_s),
          lux(l),
          duty_cycle(d_c),
          lux_reference(l_r){}
};

class System
{

private:

    /** Number of nodes in the system */
    size_t nodes_;
    /** Sampling period */
    float sample_period_;
    /** Registered entries */
    std::vector < std::vector< Entry > > entries_;
    /** Current occupancy for each desk */
    std::vector< bool > occupancy_;
    /** Illuminance lower bound for each desk */
    std::vector< float > lux_lower_bound_;
    /** External illuminance for each desk */
    std::vector< float > lux_external_;

public:

    /**
     * @brief      Constructor
     *
     * @param[in]  nodes  The number of nodes in the system
     * @param[in]  t_s    The system input sampling period
     */
    System(size_t nodes, float t_s)
        : nodes_(nodes),
          sample_period_(t_s),
          entries_(nodes, std::vector < Entry >()),
          occupancy_(nodes),
          lux_lower_bound_(nodes),
          lux_external_(nodes)
    {
        start();
    }

    // TODO
    void start();

    // TODO
    void readData();

    /**
     * @brief      Inserts a log entry
     *
     * @param[in]  id             The identifier
     * @param[in]  timestamp      The timestamp
     * @param[in]  lux            The lux
     * @param[in]  duty_cycle     The duty cycle
     * @param[in]  lux_reference  The lux reference
     */
    void insertEntry(
        size_t id,
        std::time_t timestamp,
        float lux,
        int duty_cycle,
        float lux_reference);

    /* Get */

    /**
     * @brief      Gets the latest lux value for a given desk.
     *
     * @param[in]  id    The node identifier
     *
     * @return     The lux.
     */
    float getLux(size_t id);

    /**
     * @brief      Gets the latest duty cycle for a given desk.
     *
     * @param[in]  id    The node identifier
     *
     * @return     The duty cycle.
     */
    int getDutyCycle(size_t id);

    /**
     * @brief      Gets the occupancy for a given desk.
     *
     * @param[in]  id    The node identifier
     *
     * @return     The occupancy.
     */
    bool getOccupancy(size_t id);

    /**
     * @brief      Gets the illuminance lower bound for a given desk.
     *
     * @param[in]  id    The node identifier
     *
     * @return     The lux lower bound.
     */
    float getLuxLowerBound(size_t id);

    /**
     * @brief      Gets the external illuminance for a given desk.
     *
     * @param[in]  id    The node identifier
     *
     * @return     The lux external.
     */
    float getLuxExternal(size_t id);

    /**
     * @brief      Gets the reference illuminance for a given desk.
     *
     * @param[in]  id    The node identifier
     *
     * @return     The lux reference.
     */
    float getLuxReference(size_t id);

    /**
     * @brief      Gets the power for a given desk or the total power.
     *
     * @param[in]  id     The node identifier
     * @param[in]  total  Whether to calculate total power
     *
     * @return     The power.
     */
    float getPower(size_t id, bool total);

    /**
     * @brief      Computes the energy for a given node.
     *
     * @param[in]  id    The node identifier
     *
     * @return     The energy.
     */
    float energyNode(size_t id);

    /**
     * @brief      Gets the energy for a given desk or total energy.
     *
     * @param[in]  id     The node identifier
     * @param[in]  total  Whether to calculate total energy
     *
     * @return     The energy.
     */
    float getEnergy(size_t id, bool total);

    /**
     * @brief      Computes the comfort error for a given node.
     *
     * @param[in]  id    The node identifier
     *
     * @return     The comfort error.
     */
    float comfortErrorNode(size_t id);

    /**
     * @brief      Gets the comfort error for a given desk or total comfort error.
     *
     * @param[in]  id     The node identifier
     * @param[in]  total  Whether to calculate total comfort error
     *
     * @return     The comfort error.
     */
    float getComfortError(size_t id, bool total);

    /**
     * @brief      Computes the comfort error for a given node.
     *
     * @param[in]  id    The node identifier
     *
     * @return     The comfort error.
     */
    float comfortVarianceNode(size_t id);

    /**
     * @brief      Gets the comfort variance for a given desk or total comfort variance.
     *
     * @param[in]  id     The node identifier
     * @param[in]  total  Whether to calculate total comfort variance
     *
     * @return     The comfort variance.
     */
    float getComfortVariance(size_t id, bool total);

    /* Set */

    void reset();

    void setOccupancy(size_t id, bool occupancy);

};

#endif