#include <iostream>
#include <string>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include "logging.hpp"
#include "globals.h"
#include "runcom_parser.hpp"
#include "feed_watcher.hpp"
#include "db_access.hpp"
#include "text_extractor.hpp"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#define VERSION "0.0.2"

#ifdef _DEBUG
# define LOG_LEVEL 2 // Max log level for debug
#else
# define LOG_LEVEL 1 // Max log level for release
#endif // !_DEBUG

void usage(const po::options_description &desc)
{
  std::cout << "usage: newswatcher" << std::endl;
  std::cout << desc << std::endl;
}

 int test_callback(void *NotUsed, int argc, char **argv, char **azColName)
 {
 return 0;
  }

int main(int argc, char *argv[])
{
  // Initializes logging
  BOOST_LOG_INIT("[" >> boost::logging::level >> "]" 
    >> boost::logging::filename >> "(" >> boost::logging::line >> ")," 
    >> boost::logging::time >> "," >> boost::logging::trace >> boost::logging::eol, LOG_LEVEL);
  
  BOOST_LOG_ADD_OUTPUT_STREAM(new std::ofstream("./log.txt", std::ios_base::app));
#ifdef _DEBUG
  BOOST_LOG_ADD_OUTPUT_STREAM(&std::cout);
#endif // !_DEBUG

  try 
  {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "display help message")
        ("version,v", "display version")
        ("config-file,c", po::value<std::string>()->default_value("./newswatch.rc"), "specify the configuration file")
        ("watch,w", "just download new articles from feed without analysis")
    ;

    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);


    if (vm.count("version"))
    {
      BOOST_LOG(3, "Displaying version");
      std::cout << VERSION << std::endl;
      return 0;
    }

    if (vm.count("help"))
    {
      BOOST_LOG(3, "Displaying help");
      usage(desc);
      return 1;
    }

    if (vm.count("watch"))
    {
      BOOST_LOG(1, "Watch mode set");
    }

    // Parsing configuration file
    BOOST_LOG(1, "Using " << vm["config-file"].as<std::string>() << "as a config file");
    runcom_parser rc_parser(vm["config-file"].as<std::string>());
    rc_parser.parse();
    rc_parser.parse_feeds(rc_parser.get_config()->m_feed_filename);
    rc_parser.parse_cookies(rc_parser.get_config()->m_cookies_filename);
    configuration::get_instance()->print(std::cout);

    // Initialiaze database
    db_access::get_instance()->init();
	BOOST_LOG(1, "Number of newspapers in database: " << db_access::get_instance()->get_number_of_newspapers());
	BOOST_LOG(1, "Number of articles in database: " << db_access::get_instance()->get_number_of_articles());

    //db_access::get_instance()->dump_all_articles("G:\\Projects\\newswatch");
    //exit(0);

    //Text extractor object. It will register to feed_watcher event
    //text_extractor te;
    //te.register_callbacks(*(callback_handler::get_instance()));

    //Article catcher object. It will register to feed_watcher event and notify for new article in database.
    //article_catcher ca;
    //ca.register_callbacks(*(callback_handler::get_instance()));

    //Launch feed watcher
    feed_watcher fw;
    boost::thread feed_watcher_thread(boost::bind(&(feed_watcher::watch), &fw));
    feed_watcher_thread.join();
  }
  catch(std::exception& e) 
  {
    std::cerr << "error: " << e.what() << std::endl;
    BOOST_LOG(1, "ERROR: " << e.what());
    return 1;
  }
#ifndef _DEBUG
  catch(...) 
  {
    std::cerr << "Exception of unknown type!" << std::endl;
  }
#endif

  return 0;
}