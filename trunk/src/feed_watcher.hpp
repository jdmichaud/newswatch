#ifndef _FEED_WATCHER_HPP_
#define _FEED_WATCHER_HPP_


#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>
#include <string>
#include <exception>
#include <algorithm>
#include <boost/regex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>
#include "globals.h"
#include "tinyxml.h"
#include "logging.hpp"
#include "db_access.hpp"
#include "http_client.hpp"
#include "callback_handler.hpp"

class feed_leacher
{
public:
  feed_leacher(const std::string &address);
  TiXmlDocument m_xml_feed;
};

class article_retriever
{
public:
  article_retriever(const std::string &address);
  std::string m_article_content;
};

class xml_feed_visitor : public TiXmlVisitor
{
public:
  xml_feed_visitor(std::vector<feed_item> *v) : m_feed_items(v) 
  {
    assert(v);
  }

  bool VisitEnter (const TiXmlElement &element, const TiXmlAttribute *firstAttribute);
  std::vector<feed_item> *m_feed_items;
};

class feed_watcher
{
public:
  feed_watcher();
  void watch();
  int get_feeds();

private:
  unsigned int m_watch_time;
};

#endif // !_FEED_WATCHER_HPP_
