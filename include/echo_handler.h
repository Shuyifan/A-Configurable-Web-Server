#ifndef ECHO_HANDLER_H
#define ECHO_HANDLER_H
#include "request_handler.h"
#include "request.h"
#include "header.h"
#include "config_parser.h"

namespace http {
namespace server {
class EchoHandler : public RequestHandler {
public:

	EchoHandler();
	/// Construct with a directory containing files to be served.

	EchoHandler(std::map<std::string, 
	    				 http::server::handler_parameter>& dir_map);
	
	/**
	Create a echo handler according to the config file and root path.
	@param  NgixnConfig file and root path
	@return RequestHandler object that contains echo handler
	*/
	static RequestHandler* create(const NginxConfig& config, 
								  const std::string& root_path);

	virtual bool handleRequest(const request& req, std::string& response) override;
	/**
	Given a request, this function will prepare a Response object 
	@param  request obejct
	@return unique pointer of Response
	*/
	virtual std::unique_ptr<http::server::Response> HandlerRequest(const request& request) override;
};
}
}

#endif