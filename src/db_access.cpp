#include <vector>
#include <string>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/filesystem.hpp>
#include "globals.h"
#include "db_access.hpp"
#include "callback_handler.hpp"

/**
 * dbaccess implementation
 */

boost::mutex	 db_access::m_callback_mutex;
//\/:*?"<>|

db_access *db_access::get_instance()
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

int db_access::generic_callback(void *object, int argc, char **argv, char **azColName)
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

void db_access::init()
{
  std::ifstream database(configuration::get_instance()->m_article_database.c_str());
  LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_access::init: checking database (" << configuration::get_instance()->m_article_database << ") ...");
  if (SQLITE_OK != sqlite3_open(configuration::get_instance()->m_article_database.c_str(), &m_database))
  {
    LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, sqlite3_errmsg(m_database));
    return ;
  }
  if (!database.is_open())
  {
    LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_access::init: Database " << configuration::get_instance()->m_article_database << " does not exist, creating it ...");
    if (create_database())
    {
      std::string e("Error creating database");
      throw e;
    }
  }
  else
    LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_access::init: Database " << configuration::get_instance()->m_article_database << " exists");

  LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_access::init: Init newspaper database");
  for (unsigned int i = 0; i < configuration::get_instance()->m_medias.size(); ++i)
  {
    db_newspaper newspaper;
    newspaper.m_name = configuration::get_instance()->m_medias[i].m_media_name;
    if (!newspaper.exist())
    {
      LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_access::init: " << newspaper.m_name << " does not exist");
      newspaper.insert();
      callback_handler::get_instance()->new_newspaper(newspaper.m_name);
    }
  }

}

int db_access::create_database()
{
  sqlite3_exec(m_database, "CREATE DATABASE Article_Database;", NULL, NULL, NULL);
  sqlite3_exec(m_database, "CREATE TABLE newspaper (uid integer PRIMARY KEY,  \
                                            name            varchar,  \
                                            country         varchar,  \
                                            website         varchar,  \
                                            language        varchar,  \
                                            managingEditor  varchar,  \
                                            image           varchar);", NULL, NULL, NULL);
  sqlite3_exec(m_database, "CREATE TABLE article   (uid integer PRIMARY KEY,  \
                                            newspaperuid    integer,  \
                                            title           varchar,  \
                                            link            varchar,  \
                                            description     varchar,  \
                                            author          varchar,  \
                                            category        varchar,  \
                                            comments        varchar,  \
                                            enclosure       varchar,  \
                                            guid            varchar,  \
                                            publicationDate date,     \
                                            source          varchar,  \
                                            feedName        varchar,  \
                                            feedAddress     varchar,  \
                                            content         text);", NULL, NULL, NULL);

  return 0;
}

int db_access::execute(const std::string &statement)
{
  assert(m_database);
  return sqlite3_exec(m_database, statement.c_str(), NULL, NULL, NULL);
}

int db_access::execute(const std::string &statement, sqlite3_callback callback, void *argument, char **errmsg)
{
  assert(m_database);
  return sqlite3_exec(m_database, statement.c_str(), callback, argument, errmsg);
}

const char *db_access::get_last_error()
{
  assert(m_database);
  return sqlite3_errmsg(m_database);
}

int db_access::dump_all_articles(const std::string &path)
{
  std::ostringstream query;

  query << "SELECT name FROM newspaper;";
  LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_access::dump_all_articles: query -->");
  LOGLITE_LOG_(LOGLITE_LEVEL_1,query.str());
  query_result_t newspaper_names;
  if (SQLITE_OK != execute(query.str(), generic_callback, &newspaper_names, NULL))
  {
    LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, "db_access::dump_all_articles: query failed: " << get_last_error());
    return -1;
  }

  LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_access::dump_all_articles: " << newspaper_names.size() << " newspapers in the database");
  
  db_access::query_result_t::const_iterator it = newspaper_names.begin();
  for (; it != newspaper_names.end(); ++it)
  {
    std::cout << it->first << std::endl;
    for (unsigned int i = 0; i < it->second.size(); ++i)
      std::cout << "\t" << it->second[i] << std::endl;
  }

  for (unsigned int i = 0; i < newspaper_names["Name"].size(); ++i)
  {
    std::ostringstream articles_query;

    articles_query << "SELECT title, content FROM article WHERE newspaperuid='" << newspaper_names["uid"][i] << "';";
    LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_access::dump_all_articles: query -->");
    LOGLITE_LOG_(LOGLITE_LEVEL_1, articles_query.str());
    query_result_t articles;
    if (SQLITE_OK != execute(articles_query.str(), generic_callback, &articles, NULL))
    {
      LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, "db_access::dump_all_articles: query failed: " << get_last_error());
      return -1;      
    }

    for (unsigned int j = 0; j < articles["Title"].size(); ++j)
    {
      std::ostringstream filename;
      filename << newspaper_names["Name"][i] << " - " << prepare_string_for_filename(articles["Title"][j], "\\/:*?\"<>|") << ".html";
      LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_access::dump_all_articles: Writing " << filename.str() << " ...");
      std::ofstream output_file(filename.str().c_str());
      output_file << articles["Content"][j] << std::endl;
      output_file.close();
    }

  }
  return 0;
}

std::string db_access::prepare_string_for_filename(const std::string &str, const std::string &char_to_delete)
{
  std::string ret = str;

  std::string::const_iterator it = char_to_delete.begin();
  for ( ; it != char_to_delete.end(); ++it)
    for (std::size_t i = 0; (i < ret.size()) && (i = ret.find(*it, i)) != std::string::npos; i = i + 2)
      ret.replace(i, 1, "_");

  return ret;
}

int db_access::get_newspapers(std::vector<std::string> &newspapers_name)
{
  std::ostringstream query;

  query << "SELECT name FROM newspaper;";
  LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_access::get_newspapers: query -->");
  LOGLITE_LOG_(LOGLITE_LEVEL_1, query.str());
  query_result_t newspapers;
  if (SQLITE_OK != execute(query.str(), generic_callback, &newspapers, NULL))
  {
    std::cerr << get_last_error() << std::endl;
    LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_newspaper::get_access: query failed: " << get_last_error());
    return -1;      
  }

  newspapers_name = newspapers["Name"];
  return 0;
}

unsigned int db_access::get_number_of_newspapers()
{
  std::ostringstream query;

  query << "SELECT Count(name) FROM newspaper;";
  LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_access::get_number_of_newspapers: query -->");
  LOGLITE_LOG_(LOGLITE_LEVEL_1, query.str());
  query_result_t newspapers_count;
  if (SQLITE_OK != execute(query.str(), generic_callback, &newspapers_count, NULL))
  {
    LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, "db_access::get_number_of_newspapers: query failed: " << get_last_error());
    return -1;      
  }

  std::string result = newspapers_count.begin()->second[0];
  LOGLITE_LOG_(LOGLITE_LEVEL_1, "answer: " << result);
  return atoi(result.c_str());
}

int db_access::get_articles(const std::string &newspaper_name, std::vector<unsigned long int> &uids)
{
  std::ostringstream query;

  query << "SELECT uid FROM article WHERE NewspaperName='" << newspaper_name << "';";
  LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_access::get_articles: query -->");
  LOGLITE_LOG_(LOGLITE_LEVEL_1, query.str());
  query_result_t article_uids;
  if (SQLITE_OK != execute(query.str(), generic_callback, &article_uids, NULL))
  {
    LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, "db_access::get_number_of_articles: query failed: " << get_last_error());
    return -1;      
  }

  unsigned int i = 0;
  for (; i < article_uids["uid"].size(); ++i)
    uids.push_back(atol(article_uids["uid"][i].c_str()));

  return 0;
}

unsigned int db_access::get_number_of_articles()
{
  std::ostringstream query;

  query << "SELECT COUNT(title) FROM article;";
  LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_access::get_number_of_articles: query -->");
  LOGLITE_LOG_(LOGLITE_LEVEL_1, query.str());
  query_result_t article_count;
  if (SQLITE_OK != execute(query.str(), generic_callback, &article_count, NULL))
  {
    LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, "db_access::get_number_of_articles: query failed: " << get_last_error());
    return -1;      
  }

  std::string result = article_count.begin()->second[0];
  LOGLITE_LOG_(LOGLITE_LEVEL_1, "answer: " << result);
  return atoi(result.c_str());
}


/**
 * dbobject implementation
 */

std::string db_object::prepare_string_for_db(const std::string &str)
{
  std::string ret = str;
    
  for (std::size_t i = 0; (i < ret.size()) && (i = ret.find("'", i)) != std::string::npos; i = i + 2)
    ret.replace(i, 1, "''");

  return ret;
}

/**
 * db_newspaper implementation
 */

int db_newspaper::insert()
{
  std::ostringstream query;
  
  query << "INSERT INTO newspaper (NULL, name, country, website, language, managingEditor, image) VALUES (";
  query << "'" << db_object::prepare_string_for_db(m_name) << "', ";
  query << "'" << db_object::prepare_string_for_db(m_country) << "', ";
  query << "'" << db_object::prepare_string_for_db(m_website) << "', ";
  query << "'" << db_object::prepare_string_for_db(m_language) << "', ";
  query << "'" << db_object::prepare_string_for_db(m_managing_editor) << "', ";
  query << "'" << db_object::prepare_string_for_db(m_image) << "');";

  LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_newspaper::insert: query -->");
  LOGLITE_LOG_(LOGLITE_LEVEL_1, query.str());

  if (SQLITE_OK != db_access::get_instance()->execute(query.str()))
  {
    LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, "db_newspaper::insert: Insertion failed: " << db_access::get_instance()->get_last_error());
    return -1;
  }
  else
    LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_newspaper::insert: Insertion done");

  return 0;
}

int db_newspaper::exist()
{
  std::ostringstream query;
    
  query << "SELECT name FROM newspaper WHERE name='" << db_object::prepare_string_for_db(m_name) << "';";
  LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_newspaper::exist: query -->");
  LOGLITE_LOG_(LOGLITE_LEVEL_1, query.str());
  if (SQLITE_OK != db_access::get_instance()->execute(query.str(), exist_callback, this, NULL))
  {
    LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, "db_newspaper::exist: query failed: " << db_access::get_instance()->get_last_error());
    return -1;      
  }

  int ret = 0;
  if (m_exist)
    ret = 1;

  LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_newspaper::insert: newspaper : '" << m_name << (m_exist ? "' exists" : "' do not exists"));
  m_exist = false;
  return ret;
}

int db_newspaper::exist_callback(void *object, int argc, char **argv, char **azColName)
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

unsigned int db_newspaper::get_uid(const std::string &newspaper_name)
{
  std::ostringstream query;
    
  query << "SELECT uid FROM newspaper WHERE name='" << db_object::prepare_string_for_db(newspaper_name) << "';";
  LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_newspaper::get_uid: query -->");
  LOGLITE_LOG_(LOGLITE_LEVEL_1, query.str());
  db_access::query_result_t result;
  if (SQLITE_OK != db_access::get_instance()->execute(query.str(), db_access::generic_callback, &result, NULL))
  {
    LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, "db_newspaper::get_uid: query failed: " << db_access::get_instance()->get_last_error());
    return -1;      
  }
    
  if (result.find("uid") != result.end())
    return atoi(result["uid"].front().c_str());
  return 0;
}

/**
 * db_article implementation
 */

int db_article::insert_if_not_exist()
{
  boost::mutex::scoped_lock scoped_lock(m_insert_mutex);
  if (!exist()) insert();
}

int db_article::insert()
{
  int newspaperuid = db_newspaper::get_uid(m_newspaper_name);
  if (newspaperuid == 0)
  {
    LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, "Error: No sush newspaper name in database: " << m_newspaper_name);
  }
      
  std::ostringstream query;

  query << "INSERT INTO article (uid, newspaperuid, Title, Link, Description, Author, Category, Comments, Enclosure, Guid, PublicationDate, Source, FeedName, FeedAddress, Content) VALUES (NULL, ";
  query << "'" << newspaperuid << "', ";
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
  LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_article::insert: query -->");
  LOGLITE_LOG_(LOGLITE_LEVEL_1, query.str());
  query << "'" << db_object::prepare_string_for_db(m_content) << "');";
  
  if (SQLITE_OK != db_access::get_instance()->execute(query.str()))
  {
    LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, "db_article::insert: Insertion failed: " 
                << db_access::get_instance()->get_last_error());
    return -1;
  }
  else
  {
    LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_article::insert: Insertion done");
    query.str("");
    query << "SELECT uid FROM Article WHERE Title='" << db_object::prepare_string_for_db(m_title) << "';";
    LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_article::insert: query -->");
    LOGLITE_LOG_(LOGLITE_LEVEL_1, query.str());
    db_access::query_result_t result;
    if (SQLITE_OK != db_access::get_instance()->execute(query.str(), db_access::generic_callback, &result, NULL))
    {
      LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, "db_article::insert: Query failed: " << db_access::get_instance()->get_last_error());
      return -1;
    }
    else
      m_uid = atoi(result["uid"].front().c_str());

  }

  return 0;
}

int db_article::exist()
{
  std::ostringstream query;
  
  query << "SELECT Title FROM Article WHERE Title='" << db_object::prepare_string_for_db(m_title) << "';";
  LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_article::exist: query -->");
  LOGLITE_LOG_(LOGLITE_LEVEL_1, query.str());
  if (SQLITE_OK != db_access::get_instance()->execute(query.str(), exist_callback, this, NULL))
  {
    LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, "db_article::insert: query failed: " << db_access::get_instance()->get_last_error());
    return -1;      
  }

  int ret = 0;
  if (m_exist)
    ret = 1;

  LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_article::insert: article title : " << m_title << (m_exist ? " exists" : " do not exists"));
  m_exist = false;
  return ret;
}

int db_article::exist_callback(void *object, int argc, char **argv, char **azColName)
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

int db_article::get()
{
  std::ostringstream query;

  query << "SELECT * FROM Article WHERE uid='" << m_uid << "';";
  LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_article::exist: query -->");
  LOGLITE_LOG_(LOGLITE_LEVEL_1, query.str());
  db_access::query_result_t result;
  if (SQLITE_OK != db_access::get_instance()->execute(query.str(), db_access::generic_callback, &result, NULL))
  {
    LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, "db_article::insert: query failed: " << db_access::get_instance()->get_last_error());
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
  
  LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_article::get: article extracted Title: " << m_title);
  return 0;
}

/**
 * db_marker implementation
 */

int db_marker::load()
{
  std::string query;
  query = "SELECT article_uid FROM m_marker_name;";
  
  db_access::query_result_t result;
  LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_marker::load: loading marker table with query: " << query);
  if (SQLITE_OK != db_access::get_instance()->execute(query, db_access::generic_callback, &result, NULL))
  {
    LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, "db_article::insert: query failed: " << db_access::get_instance()->get_last_error());
    return -1;      
  }
    
  LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_marker::load: found " << result["article_uid"].size() << " uids for marker " << m_marker_name);
  unsigned int i = 0;
  for ( ; i < result["article_uid"].size(); ++i)
    m_article_uids.push_back(atoi(result["article_uid"][i].c_str()));

  return 0;
}

int db_marker::create()
{
  std::string query;
  query = "CREATE TABLE " + m_marker_name + " (integer article_uid);";
  LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_marker::create: Creating marker table with query: " << query);
  if (SQLITE_OK != db_access::get_instance()->execute(query))
  {
    LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, "db_marker::create: query failed: " << db_access::get_instance()->get_last_error());
    return -1;      
  }

  return 0;
}

int db_marker::add_giud(unsigned long int uid)
{
  std::ostringstream query;
  query << "INSERT INTO " << m_marker_name << " (article_uid) VALUES (" << uid << ");";
  LOGLITE_LOG_(LOGLITE_LEVEL_1, "db_marker::add_giud: adding uid: " << uid << " to " << m_marker_name << " with query: " << query);
  if (SQLITE_OK != db_access::get_instance()->execute(query.str()))
  {
    LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, "db_marker::create: query failed: " << db_access::get_instance()->get_last_error());
    return -1;      
  }

  return 0;
}

int db_marker::delete_table()
{
  return -1;
}
