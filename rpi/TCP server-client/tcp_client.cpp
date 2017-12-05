/**
 * @file tcp_client.cpp
 */

#include "tcp_client.hpp"

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: " << argv[0] << "<host> <port>" << std::endl;
            return -1;
        }

        std::string host(argv[1]);
        std::string port(argv[2]);

        boost::asio::io_service io;
        tcp::resolver res(io);
        TCPClient client(io);

        client.start(res.resolve(tcp::resolver::query(host, port)));
        io.run();
    }
    catch (std::exception & e)
    {
        errPrintTrace(e.what());
    }
    return 0;
}


void TCPClient::start(tcp::resolver::iterator endpoint_iter)
{
    // Start the TCP connect actor
    startConnect(endpoint_iter);
    // Start the console reader actor
    startReadConsole();

    // Start deadline actor
    deadline_.async_wait(boost::bind(& TCPClient::checkDeadline, this));
}

void TCPClient::stop()
{
    stopped_ = true;
    boost::system::error_code ignored_err_code;
    socket_.close(ignored_err_code);
    deadline_.cancel();
    heartbeat_timer_.cancel();
}

void TCPClient::startConnect(tcp::resolver::iterator endpoint_iter)
{
    if (endpoint_iter != tcp::resolver::iterator())
    {
        debugPrintTrace("Trying " << endpoint_iter->endpoint());
        deadline_.expires_from_now(boost::posix_time::seconds(EXPIRE_CONNECT));
        socket_.async_connect(endpoint_iter->endpoint(),
            boost::bind(& TCPClient::handleConnect,
                this, _1 , endpoint_iter));
    }
    else
    {
        // Exhausted every available endpoint
        stop();
    }
}

void TCPClient::handleConnect(const boost::system::error_code & err_code,
    tcp::resolver::iterator endpoint_iter)
{
    if (stopped_) return;

    if (!socket_.is_open())
    {
        debugPrint("Connect timed out");
        startConnect(++endpoint_iter);
    }
    else if (err_code)
    {
        errPrintTrace(err_code.message());
        socket_.close();
        startConnect(++endpoint_iter);
    }
    else
    {
        debugPrintTrace("Connected to " << endpoint_iter->endpoint());
        // Start input actor
        startRead();
        // Start heartbeat actor
        startWrite();
    }
}

void TCPClient::startRead()
{
    deadline_.expires_from_now(boost::posix_time::seconds(EXPIRE_READ));
    boost::asio::async_read_until(socket_, input_buffer_, MSG_DELIMETER,
        boost::bind(& TCPClient::handleRead, this, _1));
}

void TCPClient::handleRead(const boost::system::error_code & err_code)
{
    if (stopped_) return;

    if (!err_code)
    {
        std::string line;
        std::istream is(& input_buffer_);
        std::getline(is, line, MSG_DELIMETER);

        // Ignore empty messages (e.g. heartbeats)
        if (!line.empty())
        {
            std::cout << "Received: " << line << std::endl;
        }
        startRead();
    }
    else
    {
        errPrintTrace(err_code.message());
        stop();
    }
}

void TCPClient::startWrite()
{
    if (stopped_) return;

    // Send empty heartbeat message
    boost::asio::async_write(socket_, boost::asio::buffer("" + MSG_DELIMETER, 1),
        boost::bind(& TCPClient::handleWrite, this, _1));
}

void TCPClient::handleWrite(const boost::system::error_code & err_code)
{
    if (stopped_) return;

    if(!err_code)
    {
        heartbeat_timer_.expires_from_now(boost::posix_time::seconds(EXPIRE_HEARTBEAT));
        heartbeat_timer_.async_wait(boost::bind(& TCPClient::startWrite, this));
    }
    else
    {
        errPrintTrace( "Heartbeat: " << err_code.message());
        stop();
    }
}

void TCPClient::checkDeadline()
{
    if (stopped_) return;

    if (deadline_.expires_at() <= deadline_timer::traits_type::now())
    {
       socket_.close();
       // Set new deadline to positive infinity to prevent further action
       deadline_.expires_at(boost::posix_time::pos_infin); 
    }
    deadline_.async_wait(boost::bind(& TCPClient::checkDeadline, this));
}

void TCPClient::startReadConsole()
{
    boost::asio::async_read_until(input_, console_buffer_, MSG_DELIMETER,
        boost::bind(& TCPClient::handleReadConsole, this, _1, _2));
}

void TCPClient::handleReadConsole(const boost::system::error_code & err_code,
    size_t length)
{
    if (stopped_) return;

    if (!err_code)
    {
        std::string line;
        std::istream is(& console_buffer_);
        std::getline(is, line, MSG_DELIMETER);

        if (!line.empty())
        {
            std::cout << "Sending: " << line << std::endl;
            std::string terminated_line = line + std::string("" + MSG_DELIMETER);
            size_t n_bytes = terminated_line.size();
            terminated_line.copy(send_buffer_, n_bytes);
            boost::asio::async_write(socket_,
                boost::asio::buffer(send_buffer_, n_bytes),
                boost::bind(& TCPClient::handleSend, this, _1, _2));
        }
        startReadConsole();
    }
    else
    {
        errPrintTrace(err_code.message());
        stop();
    }
}

void TCPClient::handleSend(const boost::system::error_code & err_code,
    size_t length)
{
    if (stopped_) return;

    if (!err_code)
    {
        std::cout << "Sent " << length << " bytes" << std::endl;
    }
    else 
    {
        errPrintTrace(err_code.message());
        stop();
    }
}