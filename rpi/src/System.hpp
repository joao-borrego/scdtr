/**
 * @file    rpi/src/System.hpp
 *
 * @brief   System main class headers
 * 
 * Provides access to the distributed system.
 * Keeps track of current state and logs heuristics.
 * 
 * @author  João Borrego
 * 
 */

#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <vector>
#include <list>
#include <algorithm>
#include <chrono>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "debug.hpp"
#include "constants.hpp"
#include "communication.hpp"

/** Flag for obtaining lux values */
#define GET_LUX         0
/** Flag for obtaining duty cycle values */
#define GET_DUTY_CYCLE  1

/**
 * @brief      Class for entry.
 */
class Entry
{

public:

    /** Entry timestamp */
    unsigned long timestamp;
    /** Registered lux value */
    float lux;
    /** Registered duty cycle */
    float duty_cycle;
    /** Reference illuminance */
    float lux_reference;
    /** Comfort error */
    float c_err;
    /** Comfort variance */
    float c_var;

    /**
     * @brief      Constructs an entry.
     *
     * @param[in]  timestamp_      The timestamp
     * @param[in]  lux_            The lux
     * @param[in]  duty_cycle_     The duty cycle
     * @param[in]  lux_reference_  The lux reference
     * @param[in]  c_err_          The comfort error
     * @param[in]  c_var_          The comfort variance
     */
    Entry(
        unsigned long timestamp_,
        float lux_, 
        float duty_cycle_,
        float lux_reference_,
        float c_err_,
        float c_var_)
        : timestamp(timestamp_),
          lux(lux_),
          duty_cycle(duty_cycle_),
          lux_reference(lux_reference_),
          c_err(c_err_),
          c_var(c_var_){}
};

/**
 * @brief      Class for system.
 */
class System
{

public:

    /** System shared pointer public type definition */
    typedef boost::shared_ptr< System > ptr;

private:

    /** Number of nodes in the system */
    size_t nodes_;
    /** Sampling period */
    float sample_period_;
    /** System initialisation timestamp */
    unsigned long start_;
    
    /* Direct measurements */

    /** Mutex for thread-safe data access */
    boost::shared_mutex mutex_;
    /** Registered log entries */
    std::vector < std::vector< Entry > > entries_;

    /** Illuminance lower bound for each desk */
    std::vector< float > lux_lower_bound_;
    /** External illuminance for each desk */
    std::vector< float > lux_external_;
    /** Current occupancy for each desk */
    std::vector< bool > occupancy_;

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
    uint8_t i2c_buffer_[RECV_BUFFER];

public:

    /**
     * @brief      Constructs a system.
     *
     * @param[in]  nodes   The number of nodes in the system
     * @param[in]  t_s     The period of the system information feed
     * @param[in]  serial  The Serial port identifier
     * @param[in]  i2c     The I2C packet stream FIFO path
     */
    System(
        size_t nodes,
        float t_s,
        const std::string & serial,
        const std::string & i2c)
        : nodes_(nodes),
          sample_period_(t_s),
          entries_(nodes * STREAM_FLAGS, std::vector < Entry >()),
          lux_lower_bound_(nodes),
          lux_external_(nodes),
          occupancy_(nodes),
          serial_port_(io_serial_),
          i2c_(io_i2c_)
    {
        start(serial, i2c);
    }

    /**
     * @brief      Obtains the number of milliseconds since last reset.
     *
     * @return     The number of milliseconds since last reset.
     */
    unsigned long millis(){
        auto now = std::chrono::system_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        auto value = now_ms.time_since_epoch();
        unsigned long absolute = value.count();
        unsigned long relative = absolute - start_;
        return relative;
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
     * @param[in]  serial  The Serial port name
     * @param[in]  i2c     The I2C packet stream FIFO
     */
    void start(const std::string & serial, const std::string & i2c);

    /**
     * @brief      Resets the system.
     * 
     * Resets the physical system and clears logs.
     * 
     */
    void reset();

    /**
     * @brief      Run I2C dedicated I/O service handle
     */
    void runI2C();

    /**
     * @brief      Run Serial dedicated I/O service handle
     */
    void runSerial();

    /**
     * @brief      Starts an I2C read.
     */
    void startRead();

    /**
     * @brief      Prepares an I2C read operation
     *
     * @param[in]  error              The error
     * @param[in]  bytes_transferred  The bytes transferred
     */
    void handleRead(const boost::system::error_code & error,
        size_t bytes_transferred);

    /**
     * @brief      Starts a write to Serial.
     *
     * @param[in]  msg   The message
     */
    void startWriteSerial(const std::string & msg);

    /**
     * @brief      Handles a write to Serial
     *
     * @param[in]  error              The error
     * @param[in]  bytes_transferred  The bytes transferred
     */
    void handleWriteSerial(const boost::system::error_code & error,
        size_t bytes_transferred);

    /**
     * @brief      Inserts an entry in the log.
     *
     * @param[in]  id             The identifier
     * @param[in]  timestamp      The timestamp
     * @param[in]  lux            The lux
     * @param[in]  duty_cycle     The duty cycle
     * @param[in]  lux_reference  The lux reference
     * @param[in]  c_err          The c error
     * @param[in]  c_var          The c variable
     */
    void insertEntry(
        size_t id,
        unsigned long timestamp,
        float lux,
        float duty_cycle,
        float lux_reference,
        float c_err,
        float c_var);

    /**
     * @brief      Saves entries to disk.
     */
    void saveEntries();

    /* Get */

    /**
     * @brief      Gets the latest entry.
     *
     * @param[in]  id    The node identifier
     *
     * @return     The latest entry.
     */
    Entry *getLatestEntry(size_t id);

    /**
     * @brief      Gets the values of lux or duty cycle in a time period.
     *
     * @param[in]  id        The node identifier
     * @param[in]  start     The start
     * @param[in]  end       The end
     * @param[in]  var       The variable (LUX | DUTY_CYCLE)
     * @param      response  The output string with comma-separated values
     */
    void getValuesInPeriod(
        size_t id,
        unsigned long start,
        unsigned long end,
        char var,
        std::string & response);

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
    float getDutyCycle(size_t id);

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

    /**
     * @brief      Gets the time since last reset for a given node.
     *
     * @param[in]  id  The node identifier
     *
     * @return     The time since last reset.
     */
    unsigned long getTimestamp(size_t id);
};

#endif
