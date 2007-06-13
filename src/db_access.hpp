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

  static db_access *get_instance()
  {
#if defined(BOOST_HAS_THREADS)
    static boost::mutex m_inst_mutex;
    boost::mutex::scoped_lock scoped_lock(m_inst_mutex);
#endif // BOOST_HAS_THREADS
    static db_access        *d = NULL;

    if (!d)
    {
      d = new db_access();
      static boost::shared_ptr<db_access> s_ptr_d(d);
    }

    return d;
  }

  typedef std::map< std::string, std::vector<std::string> > query_result_t;

  static int db_access::generic_callback(void *object, int argc, char **argv, char **azColName)
  {
    assert(object);
    query_result_t *results = (query_result_t *) object;
    boost::mutex::scoped_lock scoped_lock(db_access::m_callback_mutex);
    
    for (int i = 0; i < argc; ++i)
    {
      (*results)[azColName[i]].push_back(argv[i]);
    }

    return 0;
    
  }

  void init();
  int create_database();

  int execute(const std::string &statement)
  {
    assert(m_database);
    return sqlite3_exec(m_database, statement.c_str(), NULL, NULL, NULL);
  }

  int execute(const std::string &statement, sqlite3_callback callback, void *argument, char **errmsg)
  {
    assert(m_database);
    return sqlite3_exec(m_database, statement.c_str(), callback, argument, errmsg);
  }

  const char *get_last_error()
  {
    assert(m_database);
    return sqlite3_errmsg(m_database);
  }

  int dump_all_articles(const std::string &path);
  std::string prepare_string_for_filename(const std::string &str, const std::string &char_to_delete);
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

  static std::string prepare_string_for_db(const std::string &str)
  {
    std::string ret = str;

    for (std::size_t i = 0; (i < ret.size()) && (i = ret.find("'", i)) != std::string::npos; i = i + 2)
      ret.replace(i, 1, "''");

    return ret;
  }

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

  int insert()
  {
    std::ostringstream query;

    query << "INSERT INTO Newspaper (Name, Country, Website, Language, ManagingEditor, Image) VALUES (";
    query << "'" << db_object::prepare_string_for_db(m_name) << "', ";
    query << "'" << db_object::prepare_string_for_db(m_country) << "', ";
    query << "'" << db_object::prepare_string_for_db(m_website) << "', ";
    query << "'" << db_object::prepare_string_for_db(m_language) << "', ";
    query << "'" << db_object::prepare_string_for_db(m_managing_editor) << "', ";
    query << "'" << db_object::prepare_string_for_db(m_image) << "');";

    BOOST_LOG(1, "db_newspaper::insert: query -->");
    BOOST_LOG(1,query.str());

    if (SQLITE_OK != db_access::get_instance()->execute(query.str()))
    {
      std::cerr << db_access::get_instance()->get_last_error() << std::endl;
      BOOST_LOG(1, "db_newspaper::insert: Insertion failed: " << db_access::get_instance()->get_last_error());
      return -1;
    }
    else
      BOOST_LOG(1, "db_newspaper::insert: Insertion done");

    return 0;
  }

  int exist()
  {
    std::ostringstream query;
    
    query << "SELECT Name FROM Newspaper WHERE Name='" << db_object::prepare_string_for_db(m_name) << "';";
    BOOST_LOG(1, "db_newspaper::exist: query -->");
    BOOST_LOG(1,query.str());
    if (SQLITE_OK != db_access::get_instance()->execute(query.str(), exist_callback, this, NULL))
    {
      std::cerr << db_access::get_instance()->get_last_error() << std::endl;
      BOOST_LOG(1, "db_newspaper::insert: query failed: " << db_access::get_instance()->get_last_error());
      return -1;      
    }

    int ret = 0;
    if (m_exist)
      ret = 1;

    BOOST_LOG(1, "db_newspaper::insert: newspaper : " << m_name << (m_exist ? " exists" : " do not exists"));
    m_exist = false;
    return ret;
  }

  static int exist_callback(void *object, int argc, char **argv, char **azColName)
  {
    assert(object);
    db_newspaper *newspaper = (db_newspaper *) object;
    boost::mutex::scoped_lock scoped_lock(newspaper->m_callback_mutex);
    if (argc > 0)
      newspaper->m_exist = true;
    else
      newspaper->m_exist = false;

    return 0;
  }

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

  int insert_if_not_exist()
  {
    boost::mutex::scoped_lock scoped_lock(m_insert_mutex);
    if (!exist()) insert();
  }

  int insert()
  {
    std::ostringstream query;

    query << "INSERT INTO Article (uid, NewspaperName, Title, Link, Description, Author, Category, Comments, Enclosure, Guid, PublicationDate, Source, FeedName, FeedAddress, Content) VALUES (NULL, ";
    query << "'" << db_object::prepare_string_for_db(m_newspaper_name) << "', ";
    query << "'" << db_object::prepare_string_for_db(m_title) << "', ";
    query << "'" << db_object::prepare_string_for_db(m_link) << "', ";
    query << "'" << db_object::prepare_string_for_db(m_description) << "', ";
    query << "'" << db_object::prepare_string_for_db(m_author) << "', ";
    query << "'" << db_object::prepare_string_for_db(m_category) << "', ";
    query << "'" << db_object::prepare_string_for_db(m_comments) << "', ";
    query << "'" << db_object::prepare_string_for_db(m_enclosure) << "', ";
    query << "'" << db_object::prepare_string_for_db(m_guid) << "', ";
    query << "'" << db_object::prepare_string_for_db(m_publication_date) << "', ";
    query << "'" << db_object::prepare_string_for_db(m_source) << "', ";
    query << "'" << db_object::prepare_string_for_db(m_feed_name) << "', ";
    query << "'" << db_object::prepare_string_for_db(m_feed_address) << "', ";
    BOOST_LOG(1, "db_article::insert: query -->");
    BOOST_LOG(1, query.str());
    query << "'" << db_object::prepare_string_for_db(m_content) << "');";

    if (SQLITE_OK != db_access::get_instance()->execute(query.str()))
    {
      std::cerr << db_access::get_instance()->get_last_error() << std::endl;
      BOOST_LOG(1, "db_article::insert: Insertion failed: " << db_access::get_instance()->get_last_error());
      return -1;
    }
    else
    {
      BOOST_LOG(1, "db_article::insert: Insertion done");
      query.str("");
      query << "SELECT uid FROM Article WHERE Title='" << db_object::prepare_string_for_db(m_title) << "';";
      BOOST_LOG(1, "db_article::insert: query -->");
      BOOST_LOG(1, query.str());
      db_access::query_result_t result;
      if (SQLITE_OK != db_access::get_instance()->execute(query.str(), db_access::generic_callback, &result, NULL))
      {
        std::cerr << db_access::get_instance()->get_last_error() << std::endl;
        BOOST_LOG(1, "db_article::insert: Query failed: " << db_access::get_instance()->get_last_error());
        return -1;
      }
      else
        m_uid = atoi(result["uid"].front().c_str());

    }

    return 0;
  }

  int exist()
  {
    std::ostringstream query;
    
    query << "SELECT Title FROM Article WHERE Title='" << db_object::prepare_string_for_db(m_title) << "';";
    BOOST_LOG(1, "db_article::exist: query -->");
    BOOST_LOG(1,query.str());
    if (SQLITE_OK != db_access::get_instance()->execute(query.str(), exist_callback, this, NULL))
    {
      std::cerr << db_access::get_instance()->get_last_error() << std::endl;
      BOOST_LOG(1, "db_article::insert: query failed: " << db_access::get_instance()->get_last_error());
      return -1;      
    }

    int ret = 0;
    if (m_exist)
      ret = 1;

    BOOST_LOG(1, "db_article::insert: article title : " << m_title << (m_exist ? " exists" : " do not exists"));
    m_exist = false;
    return ret;
  }

  static int exist_callback(void *object, int argc, char **argv, char **azColName)
  {
    assert(object);
    db_article *article = (db_article *) object;
    boost::mutex::scoped_lock scoped_lock(article->m_callback_mutex);
    if (argc > 0)
      article->m_exist = true;
    else
      article->m_exist = false;

    return 0;
  }

  int get()
  {
    std::ostringstream query;

    query << "SELECT * FROM Article WHERE uid='" << m_uid << "';";
    BOOST_LOG(1, "db_article::exist: query -->");
    BOOST_LOG(1,query.str());
    db_access::query_result_t result;
    if (SQLITE_OK != db_access::get_instance()->execute(query.str(), db_access::generic_callback, &result, NULL))
    {
      std::cerr << db_access::get_instance()->get_last_error() << std::endl;
      BOOST_LOG(1, "db_article::insert: query failed: " << db_access::get_instance()->get_last_error());
      return -1;      
    }

    m_newspaper_name    = result["NewspaperName"].front();
    m_title             = result["Title"].front();
    m_link              = result["Link"].front();
    m_description       = result["Description"].front();
    m_author            = result["Author"].front();
    m_category          = result["Category"].front();
    m_comments          = result["Comments"].front();
    m_enclosure         = result["Enclosure"].front();
    m_guid              = result["Guid"].front();
    m_publication_date  = result["PublicationDate"].front();
    m_source            = result["Source"].front();
    m_feed_name         = result["FeedName"].front();
    m_feed_address      = result["FeedAddress"].front();
    m_content           = result["Content"].front();

    BOOST_LOG(1, "db_article::get: article extracted Title: " << m_title);
    return 0;
  }

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

#endif // !_DB_ACCESS_H_