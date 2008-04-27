#ifndef _HTTP_CLIENT_HPP_
#define _HTTP_CLIENT_HPP_

#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>
#include <string>
#include <exception>
#include <boost/regex.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/regex.hpp>
#include "globals.h"
#include "logging.hpp"

#define HTTP_OK     200
#define HTTP_REDIR  302

class asio_http_client
{
public:
  asio_http_client(boost::asio::io_service& io_service,
      const std::string& server, const std::string& path);

private:
  void handle_resolve(const boost::system::error_code& err,
      boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

  void handle_connect(const boost::system::error_code& err,
      boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

  void handle_write_request(const boost::system::error_code& err);

  void handle_read_status_line(const boost::system::error_code& err);

  void handle_read_headers(const boost::system::error_code& err);

  void handle_read_content(const boost::system::error_code& err);

  std::string build_cookies_string(const std::string& server);
  int extract_cookie_from_header(const std::string &server);

  boost::asio::ip::tcp::resolver m_resolver;
  boost::asio::ip::tcp::socket m_socket;
  boost::asio::streambuf m_request;
  boost::asio::streambuf m_response;
public:
  std::string m_server;
  std::stringstream m_output_stream;
  std::stringstream m_header_stream;
  unsigned int m_status_code;
};

struct http_response_t
{
  http_response_t() {}
  http_response_t(unsigned int status_code,
                  const std::string &header,
                  const std::string &content) 
                : m_status_code(status_code),
                  m_header(header),
                  m_content(content)
  {
  }

  unsigned int  m_status_code;
  std::string   m_header;
  std::string   m_content;
};

class address_extractor
{
public:
  address_extractor(const std::string &address)
  {
    extract(address);
  }

  void extract(const std::string &address)
  {
    boost::smatch what;
    boost::regex address_regex("(?:http://)?([^/]*)(/.*)");
    if (!boost::regex_match(address, what, address_regex))
    {
      std::string e((std::string("invalid address: ") + address).c_str());
      throw e;
    }

    std::string tmp(what[1].first, what[1].second);
    m_server = tmp;

    std::string tmp2(what[2].first, what[2].second);
    m_address = tmp2;

  }

  std::string m_server;
  std::string m_address;
};

class http_client
{
public:
  http_client(const std::string &address) : m_address(address)
  {

  }

  void retrieve(http_response_t &response);

  std::string m_address;
};

#endif // ! _HTTP_CLIENT_HPP_
