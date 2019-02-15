#ifndef DEFAULT_HANDLER_H
#define DEFAULT_HANDLER_H
#include "request_handler.h"
#include "request.h"
#include "header.h"
#include "config_parser.h"

namespace http {
namespace server {
class DefaultHandler : public RequestHandler {
public:

	DefaultHandler();
	/// Construct with a directory containing files to be served.
	DefaultHandler(std::map<std::string, 
	    				   http::server::handler_parameter>& dir_map);

	static RequestHandler* create(const NginxConfig& config, 
								  const std::string& root_path);

	virtual bool handleRequest(const request& req, std::string& response) override;

	virtual std::unique_ptr<http::server::Response> HandlerRequest(const request& request) override;
};
}
}

#endif