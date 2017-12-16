/**
 * @file  System.cpp
 *
 * @brief System main class
 */

#include "System.hpp"

size_t System::getNodes()
{
    return nodes_;
}

void System::start(const std::string & serial, const std::string & i2c)
{
    boost::system::error_code error;

    // Initialise Serial connection
    serial_port_.open(serial, error);
    if (error){ errPrintTrace(error.message() << ": " << serial); exit(-1); }
    serial_port_.set_option(
        boost::asio::serial_port_base::baud_rate(SERIAL_BAUDRATE), error);
    if (error){ errPrintTrace(error.message()); exit(-1); }
    debugPrintTrace("Opened Serial connection with system.");

    // Initialise read from I2C fifo
    debugPrintTrace("Waiting for writer in I2C pipe" + i2c);
    i2c_fd_ = open(i2c.c_str(), O_RDONLY);
    if (i2c_fd_ == -1){
        errPrintTrace("Failed to open I2C pipe: " + i2c);
        exit(EXIT_FAILURE);
    }
    try {
        i2c_.assign(i2c_fd_);
    } catch (std::exception & e){
        errPrintTrace("Could not assign file descriptor to I2C pipe " << e.what());
        exit(EXIT_FAILURE);   
    }
    debugPrintTrace("Opened I2C FIFO for reading.");

    // Clear internal variables
    System::reset();

    // Initialise reader actor
    startRead();
    
    debugPrintTrace("System initialised.");
}

void System::reset()
{
    // Start tracking execution time
    start_ = 0;
    start_ = System::millis();
    // Clear variables, but ensure size is kept
    boost::unique_lock<boost::shared_mutex> lock(mutex_);
    for (int i = 0; i < nodes_; i++)
    {
        entries_.at(i).clear();
        entries_.resize(nodes_);
    }
    lux_lower_bound_.clear();
    lux_external_.clear();
    occupancy_.clear();
    lux_lower_bound_.resize(nodes_);
    lux_external_.resize(nodes_);
    occupancy_.resize(nodes_);
}

void System::startRead()
{
    // TODO - Replace by async_read_until
    i2c_.async_read_some(boost::asio::buffer(i2c_buffer_, RECV_BUFFER),
        boost::bind(& System::handleRead, this, 
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void System::handleRead(const boost::system::error_code & error,
    size_t bytes_transferred)
{
    if (!error)
    {
        if (bytes_transferred >= 2){

            uint8_t id = i2c_buffer_[0];
            uint8_t type = i2c_buffer_[1];
            uint8_t *data = i2c_buffer_ + 2;

            // TODO - Verify size
            if (type == INF /* && size*/){
                
                float lux, dc, lb, ext, ref;
                uint8_t occupancy;
                unsigned long timestamp;

                Communication::float_bytes fb;
                for (int i = 0; i < 5; i++){
                    for (int j = 0; j< sizeof(float); j++){
                        fb.b[j] = data[i * sizeof(float) + j];
                    }
                    switch(i){
                        case 0: lux = fb.f; break;
                        case 1: dc  = fb.f; break;
                        case 2: lb  = fb.f; break;
                        case 3: ext = fb.f; break;
                        case 4: ref = fb.f; break;
                    }
                }
                occupancy = data[5 * sizeof(float)];
                timestamp = System::millis();

                debugPrintTrace(
                    std::setprecision(2) << std::fixed <<
                    "[I2C]" << 
                    " id "  << (int) id <<
                    " lux " << lux <<
                    " dc "  << dc <<
                    " lb "  << lb <<
                    " ext " << ext <<
                    " ref " << ref << 
                    " occ " << (bool) occupancy <<
                    " t "   << timestamp);

                // Update values in memory
                try
                {
                    lux_lower_bound_.at(id) = lb;
                    lux_external_.at(id)    = ext;
                    occupancy_.at(id)       = occupancy;
                }
                catch( std::out_of_range & e)
                {
                    errPrintTrace(e.what());
                }
                insertEntry((size_t) id, timestamp, lux, dc, ref);
            }
        }
        startRead();
    }
    else {
        errPrintTrace("Lost connection to system (I2C Sniffer)!  Exiting...");
        exit(EXIT_FAILURE);
    }
}

void System::runI2C(){
    io_i2c_.run();
}

void System::runSerial(){
    io_serial_.run();
}

void System::startWriteSerial(const std::string & msg)
{
    boost::system::error_code error;
    std::string serial = msg + "\r\n";
    debugPrintTrace("[Serial] Message ready to be sent.");
    boost::asio::async_write(serial_port_, boost::asio::buffer(serial),
        boost::bind(& System::handleWriteSerial, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void System::handleWriteSerial(const boost::system::error_code & error,
    size_t bytes_transferred)
{
    if (!error)
    {
        debugPrintTrace("[Serial] Message sent.");
    }
    else
    {
        errPrintTrace(error.message());
        errPrintTrace("Lost connection to system (Serial Port)! Exiting...");
        exit(EXIT_FAILURE);
    }

}

void System::insertEntry(
    size_t id,
    unsigned long timestamp,
    float lux,
    float duty_cycle,
    float lux_reference)
{
    boost::unique_lock<boost::shared_mutex> lock(mutex_);
    try
    {
        entries_.at(id).emplace_back(timestamp, lux, duty_cycle, lux_reference);
    }
    catch (const std::out_of_range & e)
    {
        errPrintTrace(e.what());
    }
}

Entry *System::getLatestEntry(size_t id)
{
    boost::shared_lock<boost::shared_mutex> lock(mutex_);
    try
    {
        return (entries_.at(id).empty())?
            nullptr : &entries_.at(id).back();
    }
    catch (const std::out_of_range & e)
    {
        errPrintTrace(e.what());
        return nullptr;
    }
}

void System::getValuesInPeriod(
    size_t id,
    unsigned long start,
    unsigned long end,
    int var,
    std::string & response)
{
    response = "";
    std::string value = "";

    boost::shared_lock<boost::shared_mutex> lock(mutex_);
    try
    {
        for (auto & e : entries_.at(id))
        {
            if (e.timestamp >= start && e.timestamp <= end)
            {
                if (var == GET_LUX) value = std::to_string(e.lux);
                else                value = std::to_string(e.duty_cycle);
                
                response += (value + ", "); 
            }
        }
        if (!response.empty()) response.erase(response.size() - 2);
    }
    catch (const std::out_of_range & e)
    {
        errPrintTrace(e.what());
    }

}

float System::getLux(size_t id)
{
    boost::shared_lock<boost::shared_mutex> lock(mutex_);
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

float System::getDutyCycle(size_t id)
{
    boost::shared_lock<boost::shared_mutex> lock(mutex_);
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
    boost::shared_lock<boost::shared_mutex> lock(mutex_);
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
    boost::shared_lock<boost::shared_mutex> lock(mutex_);
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
    boost::shared_lock<boost::shared_mutex> lock(mutex_);
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
    boost::shared_lock<boost::shared_mutex> lock(mutex_);
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
            power_i = getDutyCycle(i);
            power += power_i;
            if (power_i == -1) return -1;
        }
    }
    return power;
}

float System::energyNode(size_t id)
{
    float d_prev, energy = 0.0;
    unsigned long t_cur, t_prev, t_diff;
    boost::shared_lock<boost::shared_mutex> lock(mutex_);
    
    int length = entries_.at(id).size();
    for (int i = 1; i < length; i++)
    {
        d_prev = entries_.at(id).at(i - 1).duty_cycle;
        t_prev = entries_.at(id).at(i - 1).timestamp;
        t_cur  = entries_.at(id).at(i).timestamp;
        t_diff = t_cur - t_prev; // ms
        energy += d_prev * (t_diff / 1000.0);
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
    boost::shared_lock<boost::shared_mutex> lock(mutex_);

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
    boost::shared_lock<boost::shared_mutex> lock(mutex_);

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

unsigned long System::getTimestamp(size_t id)
{
    boost::shared_lock<boost::shared_mutex> lock(mutex_);
    try
    {
        return (entries_.at(id).empty())?
            -1 : entries_.at(id).back().timestamp;
    }
    catch (const std::out_of_range & e)
    {
        errPrintTrace(e.what());
        return -1;
    }
}
