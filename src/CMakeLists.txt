PROJECT(newswatch)

INCLUDE_DIRECTORIES($ENV{BOOST_INCLUDE})
LINK_DIRECTORIES($ENV{BOOST_INCLUDE}/stage/lib)
ADD_DEFINITIONS(-DTIXML_USE_STL -D_DEBUG)

ADD_EXECUTABLE(newswatch article_catcher.cpp   extract_text.hpp  http_client.hpp    sqlite3.c    symbol.hpp          tinyxml.cpp
                         article_catcher.hpp   feed_watcher.cpp  logging.hpp        text_extractor.cpp  tinyxmlerror.cpp
                         callback_handler.hpp  feed_watcher.hpp  main.cpp           sqlite3.h    text_extractor.hpp  tinyxml.h
                         db_access.cpp         globals.h         runcom_parser.cpp  symbol.cpp   tinystr.cpp         tinyxmlparser.cpp
                         db_access.hpp         http_client.cpp   runcom_parser.hpp  symbol.h     tinystr.h           word_counter.hpp)

TARGET_LINK_LIBRARIES(newswatch boost_thread-gcc42-mt-d 
                                boost_date_time-gcc42-mt-d 
                                boost_system-gcc42-mt-d
                                boost_program_options-gcc42-mt-d
                                boost_regex-gcc42-mt-d
                                pthread
                                dl
                     )
