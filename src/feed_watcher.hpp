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
  feed_leacher(const std::string &address)
  {
    boost::smatch what;
    boost::regex address_regex("(?:http://)?([^/]*)(/.*)");
    if (!boost::regex_match(address, what, address_regex))
    {
      std::exception e((std::string("invalid address: ") + address).c_str());
      throw e;
    }

    std::string server(what[1].first, what[1].second);
    std::string add(what[2].first, what[2].second);
    BOOST_LOG(2, "feed_leacher: server == " << server << " address: " << add);

    try
    {
      boost::asio::io_service io_service;
      asio_http_client c(io_service, server, add);
      io_service.run();

      c.m_output_stream >> m_xml_feed;
    }
    catch (std::exception& e)
    {
      std::cout << "feed_leacher: exception: " << e.what() << "\n";
      BOOST_LOG(1, "feed_leacher: exception: " << e.what());
    }
  }

  TiXmlDocument m_xml_feed;
};

class article_retriever
{
public:
  article_retriever(const std::string &address)
  {
    try
    {
      http_client http_c(address);
      http_response_t http_r;
      http_c.retrieve(http_r);

      for (unsigned int loop_count = 0;
           (http_r.m_status_code == HTTP_REDIR) && loop_count < 5;
           ++loop_count)
      {
        BOOST_LOG(1, "article_retriever: WARNING: Page moved (loop_count = " << loop_count << ")");
        boost::regex location_catcher("Location: (http://.*)"); 
        boost::smatch what;

        if (!boost::regex_search(http_r.m_header, what, location_catcher, boost::regex_constants::match_not_dot_newline))
        {
          BOOST_LOG(1, "article_retriever: can't extract redirection");
          std::exception e((std::string("invalid address: ") + address).c_str());
          throw e;
        }

        std::string hint_address(what[1].first, what[1].second);
        BOOST_LOG(1, "article_retriever: hint address " << hint_address);
        http_c.m_address = hint_address;
        http_c.retrieve(http_r);

        if (http_r.m_status_code == HTTP_OK)
          break;

        std::string redirection(http_r.m_content);
        boost::regex redirection_catcher("<TITLE>Redirect to (http://.*)</TITLE>"); 

        if (!boost::regex_search(redirection, what, redirection_catcher, boost::regex_constants::match_not_dot_newline))
        {
          BOOST_LOG(1, "article_retriever: can't extract redirection");
          std::exception e((std::string("invalid address: ") + hint_address).c_str());
          throw e;
        }

        std::string new_address(what[1].first, what[1].second);
        BOOST_LOG(1, "article_retriever: new location is " << new_address);
        http_c.m_address = new_address;
        http_c.retrieve(http_r);
      }

      m_article_content = http_r.m_content; // Got to do it smartly in the future
    }
    catch (std::exception& e)
    {
      std::cout << "article_retriever: exception: " << e.what() << "\n";
      BOOST_LOG(1, "article_retriever: exception: " << e.what());
    }
  }

  std::string m_article_content;
};

class xml_feed_visitor : public TiXmlVisitor
{
public:
  xml_feed_visitor(std::vector<feed_item> *v) : m_feed_items(v) 
  {
    assert(v);
  }

  bool VisitEnter (const TiXmlElement &element, const TiXmlAttribute *firstAttribute)
  {
    if (element.ValueStr() == "item")
    {
      const TiXmlNode *node;
      feed_item fi;

      if (!(node = element.FirstChild("title")))
      {
        std::exception e("malformed xml feed");
        throw e;
      }
      fi.m_title = node->ToElement()->GetText ();

      if (!(node = element.FirstChild("link")))
      {
        std::exception e("malformed xml feed");
        throw e;
      }
      fi.m_link = node->ToElement()->GetText ();

      // Optionnal values
      const char *text;
      if (node = element.FirstChild("description"))
        if (text = node->ToElement()->GetText ())
          fi.m_description = text;
      if (node = element.FirstChild("author"))
        if (text = node->ToElement()->GetText ())
          fi.m_author = text;
      if (node = element.FirstChild("category"))
        if (text = node->ToElement()->GetText ())
          fi.m_category = text;
      if (node = element.FirstChild("comments"))
        if (text = node->ToElement()->GetText ())
          fi.m_comments = text;
      if (node = element.FirstChild("enclosure"))
        if (text = node->ToElement()->GetText ())
          fi.m_enclosure = text;
      if (node = element.FirstChild("guid"))
        if (text = node->ToElement()->GetText ())
          fi.m_guid = text;
      if (node = element.FirstChild("publication_date"))
        if (text = node->ToElement()->GetText ())
          fi.m_publication_date = text;
      if (node = element.FirstChild("source"))
        if (text = node->ToElement()->GetText ())
          fi.m_source = text;

      m_feed_items->push_back(fi);
    }
    return true;
  }

  std::vector<feed_item> *m_feed_items;
};

class feed_watcher
{
public:
  feed_watcher()
  {
    if (configuration::get_instance()->m_watch_cycle != "")
      m_watch_time = atoi(configuration::get_instance()->m_watch_cycle.c_str());
    else
      m_watch_time = 300; // 5 minutes
  }

  void watch()
  {
    while (1)
    {
      get_feeds();
      boost::xtime xt;
      boost::xtime_get(&xt, boost::TIME_UTC);
      xt.sec += m_watch_time;
      BOOST_LOG(2, "feed_watcher: Gonna sleep for " << m_watch_time << " seconds now");
      boost::thread::sleep(xt); // Sleep for 1 second
    }
  }

  int get_feeds()
  {
    BOOST_LOG(1, "Get the feeds...");
    configuration *conf = configuration::get_instance();
    for (unsigned int i = 0; i < conf->m_medias.size(); ++i)
      for (unsigned int j = 0; j < conf->m_medias[i].m_feeds.size(); ++j)
      {
        feed_leacher f(conf->m_medias[i].m_feeds[j].m_feed_address);
        std::vector<feed_item> items;
        xml_feed_visitor visitor(&items);
        f.m_xml_feed.Accept(&visitor);
        
        /*
         * Items present in items and not in m_media.m_items are new
         * and shall be downloaded
         */
        std::vector<feed_item> new_items;        
        for (unsigned int m = 0; m < items.size(); ++m)
        {
          bool found = false;
          for (unsigned int n = 0; !found && n < conf->m_medias[i].m_items.size(); ++n)
          {
            if (items[m].m_title == conf->m_medias[i].m_items[n].m_title)
              found = true;
          }

          if (!found)
          {
            new_items.push_back(items[m]);
            BOOST_LOG(1, "get_feeds: new article: \"" << items[m].m_title << "\" for the " << conf->m_medias[i].m_media_name);
            db_article article;
            article.m_title = items[m].m_title;
            if (!article.exist())
            {
              article.m_newspaper_name = conf->m_medias[i].m_media_name;
              article.m_feed_name = conf->m_medias[i].m_feeds[j].m_feed_name;
              article.m_feed_address = conf->m_medias[i].m_feeds[j].m_feed_address;
              article.m_link = items[m].m_link;
              article.m_description = items[m].m_description;
              article.m_author = items[m].m_author;
              article.m_category = items[m].m_category;
              article.m_comments = items[m].m_comments;
              article.m_enclosure = items[m].m_enclosure;
              article.m_guid = items[m].m_guid;
              article.m_publication_date = items[m].m_publication_date;
              article.m_source = items[m].m_source;
            
              BOOST_LOG(1, "get_feeds: downloading " << items[m].m_link);
              article_retriever ar(items[m].m_link);
              article.m_content = ar.m_article_content; // Got to do it smartly in the future...
              article.insert();

              callback_handler::get_instance()->new_article(article.m_uid);
            }
          }
        }

        conf->m_medias[i].m_items.clear();
        conf->m_medias[i].m_items.resize(items.size());
        std::copy(items.begin(), items.end(), conf->m_medias[i].m_items.begin());
      }

    return 0;
  }

private:
  unsigned int m_watch_time;
};

#endif // !_FEED_WATCHER_HPP_