#include <boost/asio.hpp>
#include <boost/asio/error.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/find.hpp>
#include "http_client.hpp"

using boost::asio::ip::tcp;

asio_http_client::asio_http_client(boost::asio::io_service& io_service,
                         const std::string& server, const std::string& path)
                         : m_resolver(io_service),
                         m_socket(io_service)
{
  m_server = server;

  // Form the request. We specify the "Connection: close" header so that the
  // server will close the socket after transmitting the response. This will
  // allow us to treat all data up until the EOF as the content.
  std::ostream m_requeststream(&m_request);
  m_requeststream << "GET " << path << " HTTP/1.0\r\n";
  m_requeststream << "Host: " << server << "\r\n";
  m_requeststream << "Cookie: " << build_cookies_string(server) << "\r\n";
  m_requeststream << "Accept: */*\r\n";
  m_requeststream << "Connection: close\r\n\r\n";


  // Start an asynchronous resolve to translate the server and service names
  // into a list of endpoints.
  tcp::resolver::query query(server, "http");
  m_resolver.async_resolve(query,
    boost::bind(&asio_http_client::handle_resolve, this,
      boost::asio::placeholders::error,
      boost::asio::placeholders::iterator));
}

void asio_http_client::handle_resolve(const boost::system::error_code& err,
                                      tcp::resolver::iterator endpoint_iterator)
{
  if (!err)
  {
    // Attempt a connection to the first endpoint in the list. Each endpoint
    // will be tried until we successfully establish a connection.
    boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
    m_socket.async_connect(endpoint,
      boost::bind(&asio_http_client::handle_connect, this,
      boost::asio::placeholders::error, ++endpoint_iterator));
  }
  else
  {
    std::cout << "Error: " << err << std::endl;
    BOOST_LOG(1, "Error: " << err);
  }
}

void asio_http_client::handle_connect(const boost::system::error_code& err,
                                 tcp::resolver::iterator endpoint_iterator)
{
  if (!err)
  {
    // The connection was successful. Send the request.
    boost::asio::async_write(m_socket, m_request,
      boost::bind(&asio_http_client::handle_write_request, this,
      boost::asio::placeholders::error));
  }
  else if (endpoint_iterator != tcp::resolver::iterator())
  {
    // The connection failed. Try the next endpoint in the list.
    m_socket.close();
    boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
    m_socket.async_connect(endpoint,
      boost::bind(&asio_http_client::handle_connect, this,
      boost::asio::placeholders::error, ++endpoint_iterator));
  }
  else
  {
    std::cout << "Error: " << err << std::endl;
    BOOST_LOG(1, "Error: " << err);
  }
}

void asio_http_client::handle_write_request(const boost::system::error_code& err)
{
  if (!err)
  {
    // Read the response status line.
    boost::asio::async_read_until(m_socket, m_response, boost::regex("\r\n"),
      boost::bind(&asio_http_client::handle_read_status_line, this,
      boost::asio::placeholders::error));
  }
  else
  {
    std::cout << "Error: " << err << std::endl;
    BOOST_LOG(1, "Error: " << err);
  }
}

void asio_http_client::handle_read_status_line(const boost::system::error_code &err)
{
  if (!err)
  {
    // Check that response is OK.
    std::istream m_responsestream(&m_response);
    std::string http_version;
    m_responsestream >> http_version;
    
    m_responsestream >> m_status_code;
    std::string status_message;
    std::getline(m_responsestream, status_message);
    if (!m_responsestream || http_version.substr(0, 5) != "HTTP/")
    {
      std::cout << "Invalid response" << std::endl;
      BOOST_LOG(1, "Invalid response");
      return;
    }
    if (m_status_code != 200 && m_status_code != 302)
    {
      std::cout << "ERROR: Response returned with status code ";
      std::cout << m_status_code << std::endl;
      BOOST_LOG(1, "ERROR: Response returned with status code: " << m_status_code);
      return;
    }

    // Read the response headers, which are terminated by a blank line.
    boost::asio::async_read_until(m_socket, m_response, boost::regex("\r\n\r\n"),
      boost::bind(&asio_http_client::handle_read_headers, this,
      boost::asio::placeholders::error));
  }
  else
  {
    std::cout << "Error: " << err << std::endl;
    BOOST_LOG(1, "Error: " << err);
  }
}

void asio_http_client::handle_read_headers(const boost::system::error_code& err)
{
  if (!err)
  {
    // Process the response headers.
    std::istream m_responsestream(&m_response);
    std::string header;
    while (std::getline(m_responsestream, header) && header != "\r")
      m_header_stream << header << std::endl;

    extract_cookie_from_header(m_server);

    // Write whatever content we already have to output.
    if (m_response.size() > 0)
      m_output_stream << &m_response;

    // Start reading remaining data until EOF.
    boost::asio::async_read(m_socket, m_response,
      boost::asio::transfer_at_least(1),
      boost::bind(&asio_http_client::handle_read_content, this,
      boost::asio::placeholders::error));
  }
  else
  {
    std::cout << "Error: " << err << std::endl;
    BOOST_LOG(1, "Error: " << err);
  }
}

void asio_http_client::handle_read_content(const boost::system::error_code& err)
{
  if (!err)
  {
    // Write all of the data that has been read so far.
    m_output_stream << &m_response;

    // Continue reading remaining data until EOF.
    boost::asio::async_read(m_socket, m_response,
      boost::asio::transfer_at_least(1),
      boost::bind(&asio_http_client::handle_read_content, this,
      boost::asio::placeholders::error));
  }
  else if (err != boost::asio::error::eof)
  {
    std::cout << "Error: " << err << std::endl;
    BOOST_LOG(1, "Error: " << err);
  }
}

std::string asio_http_client::build_cookies_string(const std::string &server)
{
  configuration *conf = configuration::get_instance();
  std::ostringstream str;
  bool first = true;
  for (unsigned int i = 0; i < conf->m_cookies.size(); ++i)
  {
    if (conf->m_cookies[i].m_website == server
      || (std::string("www") + conf->m_cookies[i].m_website == server))
    {
      if (!first)
        str << "; ";
      str << conf->m_cookies[i].m_name << "=" << conf->m_cookies[i].m_value;
      first = false;
    }
  }

  BOOST_LOG(2, "asio_http_client::build_cookies_string: " << str.str());
  return str.str();
}

int asio_http_client::extract_cookie_from_header(const std::string &server)
{
  // Parse header for new cookies to set
  std::string line;
  configuration *conf = configuration::get_instance();
  while (std::getline(m_header_stream, line))
  {
    std::string set_cookie_string("Set-Cookie: ");
    boost::iterator_range<std::string::iterator> range = boost::find_first(line, set_cookie_string);
    if (range.begin() != range.end())
    {
      BOOST_LOG(2, "asio_http_client::extract_cookie_from_header: headers specify cookie(s)");
      BOOST_LOG(2, "asio_http_client::extract_cookie_from_header: cookies: " << line);
      std::vector<std::string> cookies;
      boost::split(cookies, line.substr(set_cookie_string.size(), line.size() - set_cookie_string.size() - 1), boost::is_any_of(";"), boost::token_compress_on);
      for (unsigned int i = 0; i < cookies.size();  ++i)
      {
        std::size_t equal_pos;
        if ((equal_pos = cookies[i].find_first_of("=")) == std::string::npos)
        {
          BOOST_LOG(1, "asio_http_client::extract_cookie_from_header: not a cookie: " << cookies[i]);
          continue;
        }

        BOOST_LOG(3, "asio_http_client::extract_cookie_from_header: cookie : " << cookies[i] << " equal_pos = " << equal_pos);
        std::string name = cookies[i].substr(0, equal_pos);
        std::string value = cookies[i].substr(equal_pos + 1);
        conf->add_cookie(server, name, value);
      }
    }
  }

  return 0;
}
