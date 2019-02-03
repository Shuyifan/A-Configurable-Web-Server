
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>

#include <boost/log/trivial.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include "server.h"
#include "session.h"

const int high_invalid_port = 65536;

server::server(int port, std::string base_dir)
    : io_service_(), base_dir_(base_dir),
	acceptor_(io_service_, tcp::endpoint(tcp::v4(), port)) {
    if(!this->is_valid(port)) {
		fprintf(stderr, "Error: Invalid port input");
		exit(1);
    }
    start_accept();
}

void server::start_accept() {
	session *new_session = new session(io_service_, base_dir_);
    acceptor_.async_accept(new_session->socket(),
                		   boost::bind(&server::handle_accept, 
						   			   this, 
									   new_session, 
									   boost::asio::placeholders::error));
}

void server::run() {
	initLogging();
	BOOST_LOG_TRIVIAL(info) << "Successfully parse config file";
	BOOST_LOG_TRIVIAL(info) << "Server starts";
	boost::asio::signal_set signals(io_service_, SIGINT);
	signals.async_wait(boost::bind(&server::handler,
							       this,
								   boost::asio::placeholders::error,
								   SIGINT));
	io_service_.run();
}

void server::handle_accept(session *new_session, const boost::system::error_code &error) {
	if(!acceptor_.is_open()) {
		return;
	}
	if(!error) {
		new_session->start();
	} else {
		printf("Connection failed: %d, %s\n", error.value(), error.message().c_str());
		delete new_session;
	}
	start_accept();
}

void server::handler(const boost::system::error_code& error, int signal_number) {
    BOOST_LOG_TRIVIAL(info) << "Server shut down";
    exit(1);
}

void server::initLogging() {
	boost::log::register_simple_formatter_factory<boost::log::trivial::severity_level, char>("Severity");

    boost::log::add_file_log(
        boost::log::keywords::file_name =
			"%Y-%m-%d-server%3N.log",
		boost::log::keywords::rotation_size =
			10 * 1024 * 1024,
		boost::log::keywords::time_based_rotation =
			boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
        boost::log::keywords::format =
			"[%TimeStamp%] [%ThreadID%] [%Severity%] %Message%"
    );

	boost::log::add_console_log(
		std::cout,
		boost::log::keywords::format = "[%TimeStamp%] [%ThreadID%] [%Severity%] %Message%"
	);

    boost::log::add_common_attributes();
}

bool server::is_valid(int port) {
	if (port >= high_invalid_port || port <= 0) {
		printf("Invalid port input.\n");
		return false;
	}  
	return true;
}