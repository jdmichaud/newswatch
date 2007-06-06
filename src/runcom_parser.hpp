#ifndef _RUNCOM_PARSER_HPP_
#define _RUNCOM_PARSER_HPP_

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
#include "logging.hpp"
#include "globals.h"

class runcom_parser
{
public:
  runcom_parser(const std::string &filename) : m_conf_filename(filename) 
  {
    m_configuration = configuration::get_instance();
  }

  int parse();
  int parse_feeds(const std::string &feed_filename);
  int parse_cookies(const std::string &cookies_filename);

  void begin_section(const char *first, const char *last)
  {
    std::string section(first, last);
    BOOST_LOG(2, "section: >" << section << "<");
    media_s media(section);
    m_configuration->m_medias.push_back(media);
  }

  void set_current_rss_name(const char *first, const char *last)
  {
    std::string feed_name(first, last);
    BOOST_LOG(2, "feed_name: >" << feed_name << "<");
    feed_s feed(feed_name);
    m_configuration->m_medias[m_configuration->m_medias.size() - 1].m_feeds.push_back(feed);
  }

  void add_rss_to_section(const char *first, const char *last)
  {
    std::string address(first, last);
    BOOST_LOG(2, "address: >" << address << "<");
    media_s *media = &m_configuration->m_medias[m_configuration->m_medias.size() - 1];
    media->m_feeds[media->m_feeds.size() - 1].m_feed_address = address;
  }

  configuration *get_config()
  {
    return m_configuration;
  }

private:
  std::string                         m_conf_filename;
  configuration                       *m_configuration;

};


#endif // !_RUNCOM_PARSER_HPP_