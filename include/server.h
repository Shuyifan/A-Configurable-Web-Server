#include <string>
#include <boost/asio.hpp>
#include <map>
#include <string>
#include "config_parser.h"
#include "handler_parameter.h"

using boost::asio::ip::tcp;

class session;

class server {
public:

    // Construct the server to listen on the specified port
    server(const NginxConfig& config);

    // Start the first accept operation.
    void start_accept();

    // Run the server's io_service loop.
    void run();

private:
    // Handle the signal to server;
    void handler(const boost::system::error_code& error, int signal_number);
    
    // Do the accept operation.
    void handle_accept(session *new_session, const boost::system::error_code &error);
    
    // Log settings
    void initLogging();

    // Check whether the port is valid.
    bool is_valid(int port);

    // The io_service used to perform asynchronous operations.
    boost::asio::io_service io_service_;
    
    // Acceptor used to listen for incoming connections.
    tcp::acceptor acceptor_;

    // The port where the server is going to listen to
    int port_;

    // The map which map the URI to a specific path of the server
    std::map<std::string, http::server::handler_parameter> dir_map_;
};