#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <string>
#include <vector>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include "logging.hpp"

struct feed_s
{
  feed_s(const std::string &feed_name) : m_feed_name(feed_name) {}
  std::string m_feed_name;
  std::string m_feed_address;
};

struct feed_item
{
  feed_item() : m_title(""), m_link(""), m_description("") {}
  feed_item(const std::string &title, const std::string &link)
    : m_title(title), m_link(link) {}

  feed_item(const std::string &title, const std::string &link, const std::string &description)
    : m_title(title), m_link(link), m_description(description) {}

  std::string m_title;
  std::string m_link;
  std::string m_description;
  std::string m_author;
  std::string m_category;
  std::string m_comments;
  std::string m_enclosure;
  std::string m_guid;
  std::string m_publication_date;
  std::string m_source;
};

struct cookie_s
{
  cookie_s() {}
  cookie_s(const std::string &website,
           const std::string &name,
           const std::string &value)
           : m_website(website),
           m_name(name),
           m_value(value)
  {}
  
  std::string m_website;
  std::string m_name;
  std::string m_value;
};

struct media_s
{
  media_s(const std::string &media_name) : m_media_name(media_name) {}

  std::string             m_media_name;
  std::vector<feed_s>     m_feeds;

  std::vector<feed_item>  m_items;
};


class configuration
{
public:
  configuration() : 
      m_feed_filename(""),
        m_article_database(""),
        m_analyzis_database(""),
        m_watch_cycle("")
      {}

  static configuration *get_instance()
  {
#if defined(BOOST_HAS_THREADS)
    static boost::mutex m_inst_mutex;
    boost::mutex::scoped_lock scoped_lock(m_inst_mutex);
#endif // BOOST_HAS_THREADS
    static configuration        *c = NULL;

    if (!c)
    {
      c = new configuration();
      static boost::shared_ptr<configuration> s_ptr_c(c);
    }

    return c;
  }

  void print(std::ostream &os)
  {
    for (unsigned int i = 0; i < m_medias.size(); ++i)
    {
      os << m_medias[i].m_media_name << std::endl;
      for (unsigned int j = 0; j < m_medias[i].m_feeds.size(); ++j)
        os << "  " << m_medias[i].m_feeds[j].m_feed_name << " : " << m_medias[i].m_feeds[j].m_feed_address << std::endl;
    }
  }

  int add_cookie(const std::string &website,
                 const std::string &name,
                 const std::string &value)
  {
    bool found = false;
    for (unsigned int i = 0; !found && (i < m_cookies.size()); ++i)
      if (m_cookies[i].m_website == website && m_cookies[i].m_name == name)
      {
        found = true;
        BOOST_LOG(2, "configuration::add_cookie: update cookie: website: " << website << " name " << name << " old value: " << m_cookies[i].m_value << " new value: " << value);
        m_cookies[i].m_value = value;
      }

    if (!found)
    {
      BOOST_LOG(1, "configuration::add_cookie: new cookie: website: " << website << " name: " << name << " value: " << value);
      m_cookies.push_back(cookie_s(website, name, value));
    }

    return 0;
  }

  std::vector<media_s>                m_medias;
  std::string                         m_feed_filename;
  std::string                         m_article_database;
  std::string                         m_analyzis_database;
  std::string                         m_watch_cycle;
  std::string                         m_cookies_filename;
  std::vector<cookie_s>               m_cookies;
  std::map<std::string, std::string>  m_unknown_config;
};

//#ifndef ___CLR___
static boost::program_options::variables_map vm;
//#endif

#endif // !_GLOBALS_H_