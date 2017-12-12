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
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "debug.hpp"
#include "constants.hpp"
#include "communication.hpp"

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

public:

    typedef boost::shared_ptr< System > ptr;

private:

    /* Logs */

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

    /* Communication handles */
    
    /** I/O Service for synchronous Serial communications */
    boost::asio::io_service io_serial_;
    /** I/O Service for asynchronous I2C communications */
    boost::asio::io_service io_i2c_;

    /* Synchronous Serial connection for outgoing commands */
    boost::asio::serial_port serial_port_;
    /** Asynchronous I2C sniffer for incoming information */
    boost::asio::posix::stream_descriptor i2c_;
    /** I2C pipe file descriptor */
    int i2c_fd_;
    /** I2C receive buffer */
    char i2c_buffer_[RECV_BUFFER];

    // TODO

public:

    /**
     * @brief      Constructor
     *
     * @param[in]  nodes  The number of nodes in the system
     * @param[in]  t_s    The system input sampling period
     * @param[in]  serial The system serial port name
     */
    System(
        size_t nodes,
        float t_s,
        const std::string & serial,
        const std::string & i2c)
        : nodes_(nodes),
          sample_period_(t_s),
          entries_(nodes, std::vector < Entry >()),
          occupancy_(nodes),
          lux_lower_bound_(nodes),
          lux_external_(nodes),
          serial_port_(io_serial_),
          i2c_(io_i2c_)
    {
        start(serial, i2c);
    }

    /**
     * @brief      Gets the number of nodes in the system.
     *
     * @return     The number of nodes in the system.
     */
    size_t getNodes();

    /**
     * @brief      Initialises System interfaces.
     *
     * @param[in]  serial  The serial port name
     */
    void start(const std::string & serial, const std::string & i2c);

    void startRead();

    void runI2C();

    // TODO
    void handleRead(const boost::system::error_code & error,
        size_t bytes_transferred);

    /**
     * @brief      Writes a message to the Serial port.
     *
     * @param[in]  msg   The message
     *
     * @return     Bytes written
     */
    int writeSerial(const std::string & msg);

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
