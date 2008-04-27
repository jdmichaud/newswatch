#ifndef _ARTICLE_CATCHER_HPP_
#define _ARTICLE_CATCHER_HPP_

#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include "logging.hpp"
#include "callback_handler.hpp"

class article_catcher;

class article_catcher_new_article_handler_t : public new_article_handler_t
{
public:
  article_catcher_new_article_handler_t() {}

  void set_article_catcher(article_catcher &te);
  void operator()();

  article_catcher *m_te;
};

class article_catcher
{
public:
  article_catcher() {}
  void register_callbacks(callback_handler &ch);
  void new_article(article_catcher_new_article_handler_t *handler);

private:
  article_catcher_new_article_handler_t m_new_article_handler;
  boost::mutex m_new_article_mutex;

};

#endif // !_ARTICLE_CATCHER_HPP_
