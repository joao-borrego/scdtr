/**
 * @file client.cpp
 * 
 * Sample TCP client as seen in
 * <a href="http://www.boost.org/doc/libs/1_45_0/doc/html/boost_asio/example/timeouts/async_tcp_client.cpp">Boost::Asio docs</a>
 */

#include <unistd.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>


using boost::asio::deadline_timer;
using boost::asio::ip::tcp;


class client
{
public:
  client(boost::asio::io_service& io_service)
    : stopped_(false),
      socket_(io_service),
      deadline_(io_service),
      heartbeat_timer_(io_service),
      input_(io_service, ::dup(STDIN_FILENO)),
      console_buffer_(100000)
  {
  }

  // Called by the user of the client class to initiate the connection process.
  // The endpoint iterator will have been obtained using a tcp::resolver.
  void start(tcp::resolver::iterator endpoint_iter)
  {
    // Start the connect actor.
    start_connect(endpoint_iter);

    // Start the read console actor
    start_read_console();

    // Start the deadline actor. You will note that we're not setting any
    // particular deadline here. Instead, the connect and input actors will
    // update the deadline prior to each asynchronous operation.
    deadline_.async_wait(boost::bind(&client::check_deadline, this));
  }

  // This function terminates all the actors to shut down the connection. It
  // may be called by the user of the client class, or by the class itself in
  // response to graceful termination or an unrecoverable error.
  void stop()
  {
    stopped_ = true;
    boost::system::error_code ignored_ec;
    socket_.close(ignored_ec);
    deadline_.cancel();
    heartbeat_timer_.cancel();
  }

private:
  void start_connect(tcp::resolver::iterator endpoint_iter)
  {
    std::cout << "start_connect" << std::endl;
    if (endpoint_iter != tcp::resolver::iterator())
    {
      std::cout << "Trying " << endpoint_iter->endpoint() << "...\n";

      // Set a deadline for the connect operation.
      deadline_.expires_from_now(boost::posix_time::seconds(60));

      // Start the asynchronous connect operation.
      socket_.async_connect(endpoint_iter->endpoint(),
          boost::bind(&client::handle_connect,
            this, _1, endpoint_iter));
    }
    else
    {
      // There are no more endpoints to try. Shut down the client.
      stop();
    }
  }

  void handle_connect(const boost::system::error_code& ec,
      tcp::resolver::iterator endpoint_iter)
  {
    std::cout << "handle_connect" << std::endl;
    if (stopped_)
      return;

    // The async_connect() function automatically opens the socket at the start
    // of the asynchronous operation. If the socket is closed at this time then
    // the timeout handler must have run first.
    if (!socket_.is_open())
    {
      std::cout << "Connect timed out\n";

      // Try the next available endpoint.
      start_connect(++endpoint_iter);
    }

    // Check if the connect operation failed before the deadline expired.
    else if (ec)
    {
      std::cout << "Connect error: " << ec.message() << "\n";

      // We need to close the socket used in the previous connection attempt
      // before starting a new one.
      socket_.close();

      // Try the next available endpoint.
      start_connect(++endpoint_iter);
    }

    // Otherwise we have successfully established a connection.
    else
    {
      std::cout << "Connected to " << endpoint_iter->endpoint() << "\n";

      // Start the input actor.
      start_read();

      // Start the heartbeat actor.
      start_write();
    }
  }

  void start_read()
  {
    std::cout << "start_read" << std::endl;
    // Set a deadline for the read operation.
    deadline_.expires_from_now(boost::posix_time::seconds(30));

    // Start an asynchronous operation to read a newline-delimited message.
    boost::asio::async_read_until(socket_, input_buffer_, '\n',
        boost::bind(&client::handle_read, this, _1));
  }

  void handle_read(const boost::system::error_code& ec)
  {
    if (stopped_)
      return;

    if (!ec)
    {
      // Extract the newline-delimited message from the buffer.
      std::string line;
      std::istream is(&input_buffer_);
      std::getline(is, line);

      // Empty messages are heartbeats and so ignored.
      if (!line.empty())
      {
        std::cout << "Received: " << line << "\n";
      }

      start_read();
    }
    else
    {
      std::cout << "Error on receive: " << ec.message() << "\n";

      stop();
    }
  }

  void start_write()
  {
    if (stopped_)
      return;

    // Start an asynchronous operation to send a heartbeat message.
    boost::asio::async_write(socket_, boost::asio::buffer("\n", 1),
        boost::bind(&client::handle_write, this, _1));
  }

  void handle_write(const boost::system::error_code& ec)
  {
    if (stopped_)
      return;

    if (!ec)
    {
      // Wait 10 seconds before sending the next heartbeat.
      heartbeat_timer_.expires_from_now(boost::posix_time::seconds(10));
      heartbeat_timer_.async_wait(boost::bind(&client::start_write, this));
    }
    else
    {
      std::cout << "Error on heartbeat: " << ec.message() << "\n";

      stop();
    }
  }

  void check_deadline()
  {
    if (stopped_)
      return;

    // Check whether the deadline has passed. We compare the deadline against
    // the current time since a new asynchronous operation may have moved the
    // deadline before this actor had a chance to run.
    if (deadline_.expires_at() <= deadline_timer::traits_type::now())
    {
      // The deadline has passed. The socket is closed so that any outstanding
      // asynchronous operations are cancelled.
      socket_.close();

      // There is no longer an active deadline. The expiry is set to positive
      // infinity so that the actor takes no action until a new deadline is set.
      deadline_.expires_at(boost::posix_time::pos_infin);
    }

    // Put the actor back to sleep.
    deadline_.async_wait(boost::bind(&client::check_deadline, this));
  }
  
  void start_read_console()
  {
     boost::asio::async_read_until(input_, console_buffer_, '\n', 
             boost::bind(&client::handle_read_console, this, _1, _2));
  }
  
  void handle_read_console(const boost::system::error_code& ec, std::size_t length)
  { 
    if (stopped_)
      return;
    
    if (!ec)
    {
      // Extract the newline-delimited message from the buffer.
      std::string line, terminated_line;
      std::istream is(&console_buffer_);
      std::getline(is, line);

      // Empty messages are heartbeats and so ignored.
      if (!line.empty())
      {
        std::cout << "Sending: " << line << "\n";
        terminated_line = line + std::string("\n");
        std::size_t n = terminated_line.size();
        terminated_line.copy(send_buffer_, n);
        boost::asio::async_write(socket_, boost::asio::buffer(send_buffer_,n), 
           boost::bind(&client::handle_send, this, _1, _2));
        
      }
      /*if (length != 0)
      {
        boost::asio::async_write(socket_, console_buffer_, 
           boost::bind(&client::handle_send, this, _1, _2));
      }*/

      start_read_console();
    }
    else
    {
      std::cout << "Error on handle_read_console: " << ec.message() << "\n";

      stop();
    }
  }
  void handle_send(const boost::system::error_code& ec, std::size_t length)
  {
    if (stopped_)
      return;
    
    if (!ec)
    {
      std::cout << "Sent " << length << " bytes" << std::endl;
    }
    else
    {
      std::cout << "Error on handle_send: " << ec.message() << "\n";

      stop();
    }
  }
  

private:
  bool stopped_;
  tcp::socket socket_;
  boost::asio::streambuf input_buffer_;
  deadline_timer deadline_;
  deadline_timer heartbeat_timer_;
  boost::asio::posix::stream_descriptor input_;
  boost::asio::streambuf console_buffer_;
  enum {max_length=1024};
  char send_buffer_[max_length];
};

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: client <host> <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;
    tcp::resolver r(io_service);
    client c(io_service);

    c.start(r.resolve(tcp::resolver::query(argv[1], argv[2])));

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}