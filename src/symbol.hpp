#ifndef SYMBOL_SYMBOL_HXX
# define SYMBOL_SYMBOL_HXX

namespace symbol
{

  inline
  Symbol::Symbol (const std::string &s)
    :_set_node (_set.insert (s).first)
  {}

  inline const std::string&
  Symbol::name_get () const
  {
    return *_set_node;
  }

  inline Symbol::size_type
  Symbol::map_size ()
  {
    return _set.size ();
  }

  inline bool
  Symbol::operator== (const Symbol &rhs) const
  {
    return _set_node == rhs._set_node;
  }

  inline bool
  Symbol::operator!= (const Symbol &rhs) const
  {
    return !this->operator== (rhs);
  }

  // The value inserted in a `set' container is only duplicated once:
  // at insertion time. So, we can safely refer to the address of the
  // inserted string.
  inline bool
  Symbol::operator<  (const Symbol &rhs) const
  {
    return &this->name_get () < &rhs.name_get ();
  }

  inline std::ostream&
  operator<< (std::ostream &ostr, const Symbol &the)
  {
    return ostr << the.name_get ();
  }

}

#endif // !SYMBOL_SYMBOL_HXX
