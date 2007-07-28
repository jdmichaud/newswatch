#include <vector>
#include <string>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/filesystem.hpp>
#include "globals.h"
#include "db_access.hpp"
#include "callback_handler.hpp"

boost::mutex	 db_access::m_callback_mutex;
//\/:*?"<>|
void db_access::init()
{
  std::ifstream database(configuration::get_instance()->m_article_database.c_str());
  BOOST_LOG(1, "db_access::init: Checking database (" << configuration::get_instance()->m_article_database << ") ...");
  if (SQLITE_OK != sqlite3_open(configuration::get_instance()->m_article_database.c_str(), &m_database))
  {
    std::cerr << sqlite3_errmsg(m_database) << std::endl;
    return ;
  }
  if (!database.is_open())
  {
    BOOST_LOG(1, "db_access::init: Database " << configuration::get_instance()->m_article_database << " does not exist, creating it ...");
    if (create_database())
    {
      std::exception e("Error creating database");
      throw e;
    }
  }
  else
    BOOST_LOG(1, "db_access::init: Database " << configuration::get_instance()->m_article_database << " exists");

  BOOST_LOG(1, "db_access::init: Init newspaper database");
  for (unsigned int i = 0; i < configuration::get_instance()->m_medias.size(); ++i)
  {
    db_newspaper newspaper;
    newspaper.m_name = configuration::get_instance()->m_medias[i].m_media_name;
    if (!newspaper.exist())
    {
      BOOST_LOG(1, "db_access::init: " << newspaper.m_name << " does not exist");
      newspaper.insert();
      callback_handler::get_instance()->new_newspaper(newspaper.m_name);
    }
  }

}

int db_access::create_database()
{
  sqlite3_exec(m_database, "CREATE DATABASE Article_Database;", NULL, NULL, NULL);
  sqlite3_exec(m_database, "CREATE TABLE Newspaper (Name            varchar,  \
                                            Country         varchar,  \
                                            Website         varchar,  \
                                            Language        varchar,  \
                                            ManagingEditor  varchar,  \
                                            Image           varchar,     \
                                            PRIMARY KEY(Name));", NULL, NULL, NULL);
  sqlite3_exec(m_database, "CREATE TABLE Article   (uid integer PRIMARY KEY,  \
                                            NewspaperName   varchar,  \
                                            Title           varchar,  \
                                            Link            varchar,  \
                                            Description     varchar,  \
                                            Author          varchar,  \
                                            Category        varchar,  \
                                            Comments        varchar,  \
                                            Enclosure       varchar,  \
                                            Guid            varchar,  \
                                            PublicationDate date,     \
                                            Source          varchar,  \
                                            FeedName        varchar,  \
                                            FeedAddress     varchar,  \
                                            Content         text);", NULL, NULL, NULL);

  return 0;
}

int db_access::dump_all_articles(const std::string &path)
{
  std::ostringstream query;

  query << "SELECT Name FROM Newspaper;";
  BOOST_LOG(1, "db_access::dump_all_articles: query -->");
  BOOST_LOG(1,query.str());
  query_result_t newspaper_names;
  if (SQLITE_OK != execute(query.str(), generic_callback, &newspaper_names, NULL))
  {
    std::cerr << get_last_error() << std::endl;
    BOOST_LOG(1, "db_newspaper::dump_all_articles: query failed: " << get_last_error());
    return -1;      
  }

  BOOST_LOG(1, "db_access::dump_all_articles: " << newspaper_names.size() << " newspapers in the database");
  
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

    articles_query << "SELECT Title, Content FROM Article WHERE NewspaperName='" << newspaper_names["Name"][i] << "';";
    BOOST_LOG(1, "db_access::dump_all_articles: query -->");
    BOOST_LOG(1, articles_query.str());
    query_result_t articles;
    if (SQLITE_OK != execute(articles_query.str(), generic_callback, &articles, NULL))
    {
      std::cerr << get_last_error() << std::endl;
      BOOST_LOG(1, "db_newspaper::dump_all_articles: query failed: " << get_last_error());
      return -1;      
    }

    for (unsigned int j = 0; j < articles["Title"].size(); ++j)
    {
      std::ostringstream filename;
      filename << newspaper_names["Name"][i] << " - " << prepare_string_for_filename(articles["Title"][j], "\\/:*?\"<>|") << ".html";
      BOOST_LOG(1, "db_access::dump_all_articles: Writing " << filename.str() << " ...");
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

  query << "SELECT Name FROM Newspaper;";
  BOOST_LOG(1, "db_access::get_newspapers: query -->");
  BOOST_LOG(1, query.str());
  query_result_t newspapers;
  if (SQLITE_OK != execute(query.str(), generic_callback, &newspapers, NULL))
  {
    std::cerr << get_last_error() << std::endl;
    BOOST_LOG(1, "db_newspaper::get_newspapers: query failed: " << get_last_error());
    return -1;      
  }

  newspapers_name = newspapers["Name"];
  return 0;
}

unsigned int db_access::get_number_of_newspapers()
{
  std::ostringstream query;

  query << "SELECT Count(Name) FROM Newspaper;";
  BOOST_LOG(1, "db_access::get_number_of_newspapers: query -->");
  BOOST_LOG(1,query.str());
  query_result_t newspapers_count;
  if (SQLITE_OK != execute(query.str(), generic_callback, &newspapers_count, NULL))
  {
    std::cerr << get_last_error() << std::endl;
    BOOST_LOG(1, "db_newspaper::get_number_of_newspapers: query failed: " << get_last_error());
    return -1;      
  }

  std::string result = newspapers_count.begin()->second[0];
  BOOST_LOG(1, "answer: " << result);
  return atoi(result.c_str());
}

int db_access::get_articles(const std::string &newspaper_name, std::vector<unsigned long int> &uids)
{
  std::ostringstream query;

  query << "SELECT uid FROM Article WHERE NewspaperName='" << newspaper_name << "';";
  BOOST_LOG(1, "db_access::get_articles: query -->");
  BOOST_LOG(1,query.str());
  query_result_t article_uids;
  if (SQLITE_OK != execute(query.str(), generic_callback, &article_uids, NULL))
  {
    std::cerr << get_last_error() << std::endl;
    BOOST_LOG(1, "db_newspaper::get_number_of_articles: query failed: " << get_last_error());
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

  query << "SELECT Count(Title) FROM Article;";
  BOOST_LOG(1, "db_access::get_number_of_articles: query -->");
  BOOST_LOG(1,query.str());
  query_result_t article_count;
  if (SQLITE_OK != execute(query.str(), generic_callback, &article_count, NULL))
  {
    std::cerr << get_last_error() << std::endl;
    BOOST_LOG(1, "db_newspaper::get_number_of_articles: query failed: " << get_last_error());
    return -1;      
  }

  std::string result = article_count.begin()->second[0];
  BOOST_LOG(1, "answer: " << result);
  return atoi(result.c_str());
}