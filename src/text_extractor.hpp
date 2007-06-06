#ifndef _TEXT_EXTRACTOR_HPP_
#define _TEXT_EXTRACTOR_HPP_

#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include "logging.hpp"
#include "callback_handler.hpp"

class text_extractor;

class text_extractor_new_article_handler_t : public new_article_handler_t
{
public:
  text_extractor_new_article_handler_t() {}

  void set_text_extractor(text_extractor &te);
  void operator()();

  text_extractor *m_te;
};

class text_extractor
{
public:
  text_extractor() {}
  void register_callbacks(callback_handler &ch);
  void new_article(text_extractor_new_article_handler_t *handler);

private:
  text_extractor_new_article_handler_t m_new_article_handler;
  boost::mutex m_new_article_mutex;

};

#endif // !_TEXT_EXTRACTOR_HPP_