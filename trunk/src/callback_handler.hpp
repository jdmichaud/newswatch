#ifndef _CALLBACK_HANDLER_HPP_
#define _CALLBACK_HANDLER_HPP_

#include <string>
#include <vector>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include "logging.hpp"

class handler_t
{
public:
  virtual void operator()() { assert(0); }
};

class new_article_handler_t : public handler_t
{
public:
  void add_uid(unsigned int uid) { m_uids.push_back(uid); }
  std::vector<unsigned int> m_uids;  
};

class new_newspaper_handler_t : public handler_t
{
public:
  void set_name(const std::string &name) { m_newspaper_name = name; }
  std::string m_newspaper_name;  
};

class callback_handler;

class callback_handler
{
public:
  callback_handler() {}
  ~callback_handler() 
  {
    m_thread_group.join_all();
  }

  static callback_handler *get_instance()
  {
    static boost::mutex m_inst_mutex;
    boost::mutex::scoped_lock scoped_lock(m_inst_mutex);

    static callback_handler               *c = NULL;
    
    if (!c)
    {
      c = new callback_handler();
      static boost::shared_ptr<callback_handler> s_ptr_c(c);
    }
    
    return c;
  }

  void new_article(unsigned int uid)
  {
    LOGLITE_LOG_(LOGLITE_LEVEL_1, "callback_handler::new_article called");

    std::vector<new_article_handler_t *>::iterator it = m_new_article_callbacks.begin();
    for (; it != m_new_article_callbacks.end(); ++it)
    {
      (*it)->add_uid(uid);
      new_article_handler_t *ah = *it;
      m_thread_vector.push_back(m_thread_group.create_thread(boost::bind(&new_article_handler_t::operator(), ah)));
    }
  }

  void new_newspaper(const std::string &newspaper_name)
  {
    LOGLITE_LOG_(LOGLITE_LEVEL_1, "callback_handler::new_newspaper called");
    
    std::vector<new_newspaper_handler_t *>::iterator it = m_new_newspaper_callbacks.begin();
    for (; it != m_new_newspaper_callbacks.end(); ++it)
    {
      (*it)->set_name(newspaper_name);
      m_thread_vector.push_back(m_thread_group.create_thread(**it));
    }
  }

  void register_to_new_article(new_article_handler_t *f)
  {
    assert(f);
    m_new_article_callbacks.push_back(f);
  }

  void register_to_new_newspaper(new_newspaper_handler_t *f)
  {
    assert(f);
    m_new_newspaper_callbacks.push_back(f);
  }

private:
  std::vector<new_article_handler_t *> m_new_article_callbacks;
  std::vector<new_newspaper_handler_t *> m_new_newspaper_callbacks;

  boost::thread_group m_thread_group;
  std::vector<boost::thread *> m_thread_vector;
};

#endif // ! _CALLBACK_HANDLER_HPP_
