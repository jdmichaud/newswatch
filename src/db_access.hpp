#ifndef _DB_ACCESS_H_
#define _DB_ACCESS_H_

#define THREADSAFE 1 // For sqlite

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <exception>
#include <boost/algorithm/string.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/shared_ptr.hpp>
#include "logging.hpp"
#include "globals.h"
#include "sqlite3.h"

class db_access
{
public:
  db_access()
  {

  }

  ~db_access()
  {
    if (m_database)
      sqlite3_close(m_database);
  }

  static db_access *get_instance();

  typedef std::map< std::string, std::vector<std::string> > query_result_t;

  void init();
  int create_database();
  static int generic_callback(void *object, int argc, char **argv, char **azColName);

  int execute(const std::string &statement);
  int execute(const std::string &statement, sqlite3_callback callback, void *argument, char **errmsg);
  const char *get_last_error();
  int dump_all_articles(const std::string &path);
  std::string prepare_string_for_filename(const std::string &str, const std::string &char_to_delete);
  int get_articles(const std::string &newspaper_name, std::vector<unsigned long int> &uids);
  int get_newspapers(std::vector<std::string> &newspapers_name);
  unsigned int get_number_of_articles();
  unsigned int get_number_of_newspapers();

private:
  sqlite3             *m_database;
  static boost::mutex	 m_callback_mutex;
};

class db_object
{
public:
  virtual int insert() = 0;
  virtual int exist() = 0;

  static std::string prepare_string_for_db(const std::string &str);
};

class db_newspaper : public db_object
{
public:
  db_newspaper() :
    m_name(""),
    m_country(""),
    m_website(""),
    m_language(""),
    m_managing_editor(""),
    m_image("")
  {
    m_exist = false;
  }

  int insert();
  int exist();
  static int exist_callback(void *object, int argc, char **argv, char **azColName);
  static unsigned int get_uid(const std::string &newspaper_name);

  std::string m_name;
  std::string m_country;
  std::string m_website;
  std::string m_language;
  std::string m_managing_editor;
  std::string m_image;

  bool m_exist;
  boost::mutex	 m_callback_mutex;
};

class db_article : public db_object
{
public:
  db_article() :
    m_newspaper_name(""),
    m_title(""),
    m_link(""),
    m_description(""),
    m_author(""),
    m_category(""),
    m_comments(""),
    m_enclosure(""),
    m_guid(""),
    m_publication_date(""),
    m_source("")
  {
    m_exist = false;
    m_uid = -1;
  }

  int insert_if_not_exist();
  int insert();
  int exist();
  static int exist_callback(void *object, int argc, char **argv, char **azColName);
  int get();

  unsigned int m_uid;

  std::string m_newspaper_name;
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

  std::string m_feed_name;
  std::string m_feed_address;

  std::string m_content;

  bool m_exist;
  boost::mutex	 m_callback_mutex;
  boost::mutex	 m_insert_mutex;
  
};

class db_marker : public db_object
{
public:
  db_marker(const std::string &marker_name) : m_marker_name(marker_name) 
  {
    
  }

  int load();
  int create();
  int add_giud(unsigned long int uid);
  int delete_table();

private:
  std::string                     m_marker_name;
  std::vector<unsigned long int>  m_article_uids;
};

#endif // !_DB_ACCESS_H_
