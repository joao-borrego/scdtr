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
    /*
    serial_port_.open(serial, error);
    if (error){ errPrintTrace(error.message() << ": " << serial); exit(-1); }
    serial_port_.set_option(
        boost::asio::serial_port_base::baud_rate(SERIAL_BAUDRATE), error);
    if (error){ errPrintTrace(error.message()); exit(-1); }
    */

    //debugPrintTrace("Opened Serial connection with system.");

    // Initialise read from I2C fifo

    debugPrintTrace("Waiting for writer in I2C pipe" + i2c);
    i2c_fd_ = open(i2c.c_str(), O_RDONLY);
    if (i2c_fd_ == -1){
        errPrintTrace("Failed to open I2C pipe: " + i2c);
        exit(EXIT_FAILURE);
    }
    
    i2c_.assign(i2c_fd_);

    startRead();
    
    debugPrintTrace("Opened I2C FIFO for reading.");
    debugPrintTrace("System initialised.");
}

void System::startRead(){
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

            if (type == INF){
                
                float lux, dc, lb, ext, ref;
                uint8_t occupancy;
                
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
                std::time_t timestamp = std::time(nullptr);

                debugPrintTrace("[I2C]" << 
                    " id " << (int) id <<
                    " lux " << lux <<
                    " dc  " << dc <<
                    " lb  " << lb <<
                    " ext " << ext <<
                    " ref " << ref << 
                    " occ " << (bool) occupancy);

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
                insertEntry(id, timestamp, lux, dc, ref);
            }
        }

        startRead();
    }
    else {
        errPrintTrace("Lost connection to system!");
        exit(EXIT_FAILURE);
    }
}

void System::runI2C(){
    io_i2c_.run();
}

int System::writeSerial(const std::string & msg)
{
    boost::system::error_code error;
    int bytes = boost::asio::write(serial_port_, boost::asio::buffer(msg), error);
    if (error){ errPrintTrace(error.message()); }
    return bytes;
}

void System::insertEntry(
    size_t id,
    std::time_t timestamp,
    float lux,
    int duty_cycle,
    float lux_reference)
{
    Entry new_entry = Entry(timestamp, lux, duty_cycle, lux_reference);
    
    boost::unique_lock<boost::shared_mutex> lock(mutex_);
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
    boost::shared_lock<boost::shared_mutex> lock(mutex_);
    
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

void System::reset()
{
    // TODO
}

void setOccupancy(size_t id, bool occupancy)
{
    // TODO
}
