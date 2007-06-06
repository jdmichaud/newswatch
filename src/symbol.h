#ifndef SYMBOL_SYMBOL_HH
# define SYMBOL_SYMBOL_HH

# include <set>
# include <string>
# include <iostream>


/*-----------------------------------------------------------------.
| Mapping any string to a unique reference.  This allows to avoid  |
| STRCMP style comparison of strings, reference comparison is much |
| faster.                                                          |
`-----------------------------------------------------------------*/
namespace symbol {

  class Symbol
  {
    typedef std::set < std::string > string_set_type;
    typedef string_set_type::size_type size_type;

  public:
    explicit Symbol (const std::string &s);

    const std::string& name_get () const;
    static size_type map_size ();

    bool operator== (const Symbol &rhs) const;
    bool operator!= (const Symbol &rhs) const;
    bool operator<  (const Symbol &rhs) const;

  private:
    static string_set_type _set;
    const string_set_type::const_iterator _set_node;
  };

  std::ostream&
  operator<< (std::ostream &ostr, const Symbol &the);

}

#include "symbol.hpp"

#endif
