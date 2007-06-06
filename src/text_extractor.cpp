#include "logging.hpp"
#include "text_extractor.hpp"

void text_extractor_new_article_handler_t::set_text_extractor(text_extractor &te)
{
  m_te = &te;
}

void text_extractor_new_article_handler_t::operator()()
{
  assert(m_te);
  m_te->new_article(this);
}

void text_extractor::register_callbacks(callback_handler &ch)
{
  m_new_article_handler.set_text_extractor(*this);
  ch.register_to_new_article(&m_new_article_handler);
}

void text_extractor::new_article(text_extractor_new_article_handler_t *handler)
{
  assert(handler);
  boost::mutex::scoped_lock scoped_lock(m_new_article_mutex);

  unsigned int uid = handler->m_uids.back();
  handler->m_uids.pop_back();
  BOOST_LOG(1, "text_extractor::new_article: article uid: " << uid);
}
