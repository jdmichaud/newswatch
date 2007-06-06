#include "logging.hpp"
#include "article_catcher.hpp"

void article_catcher_new_article_handler_t::set_article_catcher(article_catcher &te)
{
  m_te = &te;
}

void article_catcher_new_article_handler_t::operator()()
{
  assert(m_te);
  m_te->new_article(this);
}

void article_catcher::register_callbacks(callback_handler &ch)
{
  m_new_article_handler.set_article_catcher(*this);
  ch.register_to_new_article(&m_new_article_handler);
}

void article_catcher::new_article(article_catcher_new_article_handler_t *handler)
{
  assert(handler);
  boost::mutex::scoped_lock scoped_lock(m_new_article_mutex);

  unsigned int uid = handler->m_uids.back();
  handler->m_uids.pop_back();
  BOOST_LOG(1, "article_catcher::new_article: article uid: " << uid);
}
