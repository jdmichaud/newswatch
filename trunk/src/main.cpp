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
#include "word_counter.hpp"

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
  LOGLITE_INIT("[" >> loglite::mask >> "]" 
               >> loglite::filename >> "(" >> loglite::line >> ")," 
               >> loglite::time >> "," >> loglite::trace >> loglite::eol);
  

  loglite::sink file(new std::ofstream("./newswatch.log", std::ios_base::app), LOGLITE_MASK_LEVEL_3);
  file.attach_qualifier(loglite::log);
  file.attach_qualifier(loglite::error);
  file.attach_qualifier(loglite::warning);
  LOGLITE_ADD_OUTPUT_STREAM(file);

  loglite::sink error_screen(&std::cerr, LOGLITE_MASK_LEVEL_1);
  error_screen.attach_qualifier(loglite::error);
  LOGLITE_ADD_OUTPUT_STREAM(error_screen);

#ifdef _DEBUG
  loglite::sink screen(&std::cout, LOGLITE_MASK_LEVEL_2);
  screen.attach_qualifier(loglite::log);
  LOGLITE_ADD_OUTPUT_STREAM(screen);
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
      LOGLITE_LOG_(LOGLITE_LEVEL_4, "Displaying version: " << VERSION);
      std::cout << VERSION << std::endl;
      return 0;
    }

    if (vm.count("help"))
    {
      LOGLITE_LOG_(LOGLITE_LEVEL_4, "Displaying help");
      usage(desc);
      return 1;
    }

    if (vm.count("watch"))
    {
      LOGLITE_LOG_(LOGLITE_LEVEL_1, "Watch mode set");
    }

    // Parsing configuration file
    LOGLITE_LOG_(LOGLITE_LEVEL_1, "Using " << vm["config-file"].as<std::string>() << "as a config file");
    runcom_parser rc_parser(vm["config-file"].as<std::string>());
    rc_parser.parse();
    rc_parser.parse_feeds(rc_parser.get_config()->m_feed_filename);
    rc_parser.parse_cookies(rc_parser.get_config()->m_cookies_filename);
    configuration::get_instance()->print(std::cout);

    // Initialiaze database
    db_access::get_instance()->init();
	  //BOOST_LOG(1, "Number of newspapers in database: " << db_access::get_instance()->get_number_of_newspapers());
	  //BOOST_LOG(1, "Number of articles in database: " << db_access::get_instance()->get_number_of_articles());

    // Counter how many times we find 'Obama' in the articles downloaded
    //std::string democrats_tab[] = {"Joe Biden", "Hillary Clinton", "Christopher Dodd", "John Edwards", "Mike Gravel", "Dennis Kucinich", "Barack Obama", "Bill Richardson" };
    //const size_t size = sizeof democrats_tab / sizeof democrats_tab[0];
    //std::vector<std::string> democrats(democrats_tab, democrats_tab + size);

    /*
    unsigned int democrats_count = 0;
    for ( ; democrats_count < democrats.size(); ++democrats_count)
    {
      word_counter wc(democrats[democrats_count]);
	    BOOST_LOG(1, "Counting: " << democrats[democrats_count]);
      unsigned int count = wc.count();
	    BOOST_LOG(1, "found " << count << " occurences");
    }
    */
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
    boost::thread feed_watcher_thread(boost::bind(&feed_watcher::watch, &fw));
    feed_watcher_thread.join();
  }
  catch(std::exception& e) 
  {
    LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, "ERROR: " << e.what());
    return 1;
  }
#ifndef _DEBUG
  catch(...) 
  {
    LOGLITE_LOG(LOGLITE_LEVEL_1, loglite::error, "ERROR: Exception of unknown type!");
  }
#endif

  return 0;
}
