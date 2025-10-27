/**
 ** \file misc/scoped-map.hh
 ** \brief Declaration of misc::scoped_map.
 */

#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <stdexcept>
#include <misc/concepts.hh>

namespace misc
{
// https://assignments.lre.epita.fr/source_code/project_layout/lib_misc.html
/*
 scoped_map summary in this project:
 ** a stack of maps
 ** each map on scope lvl
 ** for example each tiger function, loop, class def, makes a new scope 
 ** and they get destroyed when exiting the blocks of their "use"
*/
  template <typename Key, typename Data>
  class scoped_map
  {
  public:
    /// The maps stored in each scope.
    using map_type = std::map<Key, Data>;

    scoped_map();

    void scope_begin();
    void scope_end();

    void put(const Key& key, const Data& value);

    Data get(const Key& key) const;

    // dipsplay it human readable readable
    std::ostream& dump(std::ostream& ostr) const;

  private:
// 
    using map_type = std::map<Key, Data>;
    using maps_type = std::vector<map_type>;
    maps_type maps_;
  };

} // namespace misc

#include <misc/scoped-map.hxx>