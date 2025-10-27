/**
 ** \file misc/symbol.hxx
 ** \brief Inline implementation of misc::symbol.
 */

#pragma once

#include <misc/symbol.hh>

namespace misc
{
  inline symbol& symbol::operator=(const symbol& rhs)
  {
    // FIXME: Some code was deleted here.
    // Vince's code start
    super_type::operator=(rhs);
    return *this;
    // Vince's code end
  }

  inline bool symbol::operator==(const symbol& rhs) const
  {
    // FIXME: Some code was deleted here.
    // Vince's code start
    return super_type::operator==(rhs);
    // Vicne's code end
  }

  inline bool symbol::operator!=(const symbol& rhs) const
  {
    // FIXME: Some code was deleted here.
    // Vince's code start
    return super_type::operator!=(rhs);
    // Vince's code end
  }

  inline std::ostream& operator<<(std::ostream& ostr, const symbol& the)
  {
    // FIXME: Some code was deleted here.
    // Vince's code start
    return ostr << the.get();
    // Vince's code end
  }

} // namespace misc
