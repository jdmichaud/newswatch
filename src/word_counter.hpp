#ifndef _WORD_COUNTER_HPP_
#define _WORD_COUNTER_HPP_

#include <string>
#include <vector>
#include <iostream>
#include <boost/regex.hpp>
#include "logging.hpp"

class word_counter
{
public:
  word_counter(const std::string &expression)
  {
    m_expression = boost::regex(expression);
  }

  unsigned long count()
  {
    std::vector<std::string> newspapers;
    db_access::get_instance()->get_newspapers(newspapers);
    LOGLITE_LOG_(LOGLITE_LEVEL_1, "word_counter::count: found " << newspapers.size() << " newspaper in database");

    unsigned long word_count = 0;
    unsigned int i = 0;
    for (; i < newspapers.size(); ++i)
    {
      LOGLITE_LOG_(LOGLITE_LEVEL_1, "word_counter::count: counting for " << newspapers[i] << " ...");
      word_count += count_in_newspaper(newspapers[i]);
    }

    return word_count;
  }

  unsigned long count_in_newspaper(const std::string &newspaper_name)
  {
    std::vector<unsigned long int> uids;
    db_access::get_instance()->get_articles(newspaper_name, uids);
    LOGLITE_LOG_(LOGLITE_LEVEL_1, "word_counter::count: found " << uids.size() << " articles for " << newspaper_name);

    unsigned long word_count = 0;
    unsigned int i = 0;
    for ( ; i < uids.size(); ++i)
      word_count += count_in_article(uids[i]);

    return word_count;
  }

  unsigned long count_in_article(unsigned long int uid)
  {
    unsigned long count = 0;
    db_article article;
    article.m_uid = uid;
    article.get();

    std::string::const_iterator start, end; 
    start = article.m_content.begin(); 
    end = article.m_content.end(); 
    boost::match_results<std::string::const_iterator> what; 
    boost::match_flag_type flags = boost::match_default; 
    while(boost::regex_search(start, end, what, m_expression, flags)) 
    { 
      // update search position: 
      start = what[0].second; 

      // update flags: 
      flags |= boost::match_prev_avail; 
      flags |= boost::match_not_bob; 

      ++count;
    } 

    return count;
  }

private:
  boost::regex m_expression;
};

#endif // !_WORD_COUNTER_HPP_
