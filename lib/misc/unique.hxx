/**
 ** \file misc/unique.hxx
 ** \brief Inline implementation of misc::unique.
 */

#pragma once

#include <misc/contract.hh>
#include <misc/unique.hh>

namespace misc
{
  template <typename T, class C> unique<T, C>::unique(const data_type& s)
  // FIXME: Some code was deleted here (Initializations).
  /** \brief Following the Flyweight design pattern, set the attribute to a
       unique reference of value s. You might want to check out std::set methods
       on cppreference.com. */
  {
    // Vince's code start
    // Flyweight: use only a single instance(of symbols or..) in memory
    // so when two symbols are created with the same string they should point
    // to the same instance

    // try to insert s
    // returns a pair, iterator to element, bool if inserted or not
    auto result = object_set_instance().insert(s);
    // pointer to the *unique* isntance
    obj_ = &(*result.first);
    // Vince's code end
  }

  template <typename T, class C>
  typename unique<T, C>::object_set_type& unique<T, C>::object_set_instance()
  {
    // FIXME: Some code was deleted here (Classical Singleton pattern, a la Scott Meyers').
    /** \brief Create a persistent instance of a set which would hold each value. */

    // Vince's code start
    static object_set_type os;
    return os;
    // Vince's code end
  }

  template <typename T, class C>
  typename unique<T, C>::object_size_type unique<T, C>::object_map_size()
  {
    // FIXME: Some code was deleted here.
    // Vince's code start
    return object_set_instance().size();
    // Vince's code end
  }

  template <typename T, class C>
  inline const typename unique<T, C>::data_type& unique<T, C>::get() const
  {
    // FIXME: Some code was deleted here.
    // Vince's code start
    return *obj_;
    // Vince's code end
  }

  template <typename T, class C>
  inline unique<T, C>::operator const data_type&() const
  {
    // FIXME: Some code was deleted here (Check Typecasts overload).
    // Vince's code start
    return *obj_;
    // Vince's code end
  }

  template <typename T, class C>
  inline typename unique<T, C>::value_type&
  unique<T, C>::operator=(const value_type& rhs)
  {
    if (this != &rhs)
      obj_ = rhs.obj_;
    return *this;
  }

  template <typename T, class C>
  inline bool unique<T, C>::operator==(const value_type& rhs) const
  {
    return obj_ == rhs.obj_;
  }

  template <typename T, class C>
  inline bool unique<T, C>::operator!=(const value_type& rhs) const
  {
    return !operator==(rhs);
  }

  template <typename T, class C>
  inline bool unique<T, C>::operator<(const value_type& rhs) const
  {
    C cmp;
    assertion(obj_);
    assertion(rhs.obj_);
    return cmp(*obj_, *rhs.obj_);
  }

  template <typename T, class C>
  inline std::ostream& operator<<(std::ostream& ostr, const unique<T, C>& the)
  {
    return ostr << the.get();
  }

} // namespace misc
