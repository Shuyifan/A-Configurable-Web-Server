#include <string>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "gtest/gtest.h"
#include "request.h"
#include "static_handler.h"
#include "config_parser.h"
#include "utils.h"
#include "handler_manager.h"
#include "handler_parameter.h"

using boost::asio::ip::tcp;

class StaticHandlerTest : public ::testing::Test {
  protected:
    http::server::request request_;
    http::server::RequestHandler* requestHandler = new http::server::StaticHandler();
};

TEST_F(StaticHandlerTest, BadRequest) {
    std::unique_ptr<http::server::Response> response_;
    request_.method = "GET";
    request_.uri = "/..";
    request_.http_version_major = 1;
    request_.http_version_minor = 1;
    response_ = requestHandler->HandlerRequest(request_);
    std::string res = response_->ToString();
    EXPECT_EQ(res, "HTTP/1.1 400 Bad Request\r\n\r\n");
}

TEST_F(StaticHandlerTest, NotFound) {
    std::unique_ptr<http::server::Response> response_;
    request_.method = "GET";
    request_.uri = "/static/empty.txt/";
    request_.http_version_major = 1;
    request_.http_version_minor = 1;
    response_ = requestHandler->HandlerRequest(request_);
    std::string res = response_->ToString();
    EXPECT_EQ(res, "HTTP/1.1 404 Not Found\r\n\r\nFile Not Exist!");
}