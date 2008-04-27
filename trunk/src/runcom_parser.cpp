#include <boost/regex.hpp>
#include "runcom_parser.hpp"
#include "db_access.hpp"

using namespace boost::spirit;

int runcom_parser::parse()
{
  try
  {
    LOGLITE_LOG_(LOGLITE_LEVEL_1, "reading config file " << m_conf_filename);
    std::ifstream config_file(m_conf_filename.c_str());
    if (!config_file.is_open())
    {
      LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, "could not open file: " << m_conf_filename);
      return 1;
    }

    // Parse regex file and construct regex
    std::string line;
    while (std::getline(config_file, line))
    {
      if (line[0] == '#') //comment management
        continue;

      size_t equal_pos = line.find_first_of("=");
      if (equal_pos == std::string::npos)
        continue;
      std::string item = line.substr(0, equal_pos);
      size_t first_quote_pos = line.find_first_of("\"");
      size_t last_quote_pos = line.find_last_of("\"");
      if (first_quote_pos == std::string::npos)
        continue;
      std::string value = line.substr(first_quote_pos + 1, last_quote_pos - first_quote_pos - 1);

      LOGLITE_LOG_(LOGLITE_LEVEL_1, item << " : \"" << value << "\"");

      if (item == "feeds")
        m_configuration->m_feed_filename = value;
      else if (item == "article_database")
        m_configuration->m_article_database = value;
      else if (item == "analyzis_database")
        m_configuration->m_analyzis_database = value;
      else if (item == "watch_cycle")
        m_configuration->m_watch_cycle = value;        
      else if (item == "cookies_file")
        m_configuration->m_cookies_filename = value;
      else
      {
        LOGLITE_LOG_(LOGLITE_LEVEL_1, "Warning: " << item << " unknown config item");
        m_configuration->m_unknown_config[item] = value;
      }
    }
  }
  catch (const std::exception& e)
  {
    LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, "Error: " << e.what());
  }
  catch (...)
  {
    LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, "Unknwon exception");
  }

  return 0;
}

int runcom_parser::parse_feeds(const std::string &feed_filename)
{
  try
  {
    LOGLITE_LOG_(LOGLITE_LEVEL_1, "reading feeds file " << feed_filename);
    std::ifstream feed_file(feed_filename.c_str());
    if (!feed_file.is_open())
    {
      LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, "could not open file: " << feed_filename);
      return 1;
    }

    // Parse regex file and construct regex
    std::string file_content;
    std::getline(feed_file, file_content, (char) -1);

    if (!boost::spirit::parse(file_content.c_str(),
      //  Begin grammar
      (
      *(strlit<>("section") >> ch_p('\"') >> (*~ch_p('\"'))[bind(&runcom_parser::begin_section, this, _1, _2)] >> ch_p('\"') >>
      *(ch_p('\"') >> (*~ch_p('\"'))[bind(&runcom_parser::set_current_rss_name, this, _1, _2)] >> ch_p('\"') >> ch_p('=') >> ch_p('\"') >> (*~ch_p('\"'))[bind(&runcom_parser::add_rss_to_section, this, _1, _2)] >> ch_p('\"')) >>
      strlit<>("end section"))
      )
      ,
      //  End grammar
      space_p).full)
    {
      LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, "parse error: " << feed_filename);
    }
  }
  catch (const std::exception& e)
  {
    LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, "Error: " << e.what());
  }
  catch (...)
  {
    LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, "Unknwon exception");
  }

  return 0;
}    


int runcom_parser::parse_cookies(const std::string &cookies_filename)
{
  LOGLITE_LOG_(LOGLITE_LEVEL_1, "runcom_parser::parse_cookies_file: Parsing " << cookies_filename << " for cookies");
  std::ifstream cookies_file(cookies_filename.c_str());
  if (!cookies_file.is_open())
  {
    LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, "runcom_parser::parse_cookies_file: ERROR: " << cookies_filename << " could not be opened");
    return 1;
  }

  int line_number = 0;
  std::string line;
  while (std::getline(cookies_file, line))
  {
    ++line_number;
    LOGLITE_LOG_(LOGLITE_LEVEL_3, "runcom_parser::parse_cookies_file: " << line);
    std::size_t pos = 0;
    if ((((pos = line.find_first_not_of(" ")) != std::string::npos) && (line[pos] == '#'))
        || line.empty())
      continue;

    boost::regex re("(\\S+)\\s+\\S+\\s+\\S+\\s+\\S+\\s+\\S+\\s+(\\S+)(?:\\s+)?(\\S+)?");
    boost::smatch what;
    if (!boost::regex_match(line, what, re))
    {
      LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::warning, "runcom_parser::parse_cookies_file: invalid line: " << line);
      continue;
    }

    std::string website(what[1].first, what[1].second);
    std::string name(what[2].first, what[2].second);
    std::string value(what[3].first, what[3].second);
    configuration::get_instance()->add_cookie(website, name, value);
  }

  LOGLITE_LOG_(LOGLITE_LEVEL_1, "runcom_parser::parse_cookies_file: " << line_number << " lines in cookies file");
  return 0;
}
