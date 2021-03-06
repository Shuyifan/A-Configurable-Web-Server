#include <vector>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/thread.hpp>

#include "server.h"
#include "session.h"
#include "handler_manager.h"

const int high_invalid_port = 65536;

server::server(const NginxConfig& config)
    : io_service_(), handlerManager_(config),
	acceptor_(io_service_) {
    
	thread_pool_size_ = 2 * sysconf(_SC_NPROCESSORS_ONLN) + 2;

	for(const auto& statement : config.statements_) {
		const std::vector<std::string> tokens = statement->tokens_;
		if(tokens[0] == "listen") {
			if(tokens.size() >= 2) {
				port_ = stoi(tokens[1]);
				if(!this->is_valid(port_)) {
					fprintf(stderr, "Error: Invalid port input");
					exit(1);
				}
			}
		}
	}

	boost::asio::ip::tcp::resolver resolver(io_service_);
	boost::asio::ip::tcp::endpoint endpoint(tcp::v4(), port_);
	acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	acceptor_.bind(endpoint);
	acceptor_.listen();
}

void server::start_accept() {
	session *new_session = new session(io_service_, handlerManager_);
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
	signals.async_wait(boost::bind(&server::stop, this));
	
  	std::vector<boost::shared_ptr<boost::thread>> threads;
  	for (std::size_t i = 0; i < thread_pool_size_; i ++)
  	{
    	boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&boost::asio::io_service::run, 
																			  &io_service_)));
    	threads.push_back(thread);
  	}

  	// Wait for all threads in the pool to exit.
  	for (std::size_t i = 0; i < threads.size(); i ++) {
		threads[i]->join();
	}
}

void server::stop() {
	BOOST_LOG_TRIVIAL(info) << "Server shut down";
	io_service_.stop();
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

void server::initLogging() {
	boost::log::register_simple_formatter_factory<boost::log::trivial::severity_level, char>("Severity");

    boost::log::add_file_log(
        boost::log::keywords::file_name =
			"log/%Y-%m-%d-server%3N.log",
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