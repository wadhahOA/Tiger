/**
 ** \file misc/scoped-map.hxx
 ** \brief Implementation of misc::scoped_map.
 */

#pragma once

#include <iostream>
#include <sstream>
#include <type_traits>
#include <misc/contract.hh>

namespace misc
{
  template <typename Key, typename Data> scoped_map<Key, Data>::scoped_map()
  {
    scope_begin();
  }

  template <typename Key, typename Data>
  void scoped_map<Key, Data>::scope_begin()
  {
    maps_.push_back(map_type());
  }

  template <typename Key, typename Data> void scoped_map<Key, Data>::scope_end()
  {
    // there has to be a scope
    // (    contract.hh) literalmenet pre-condition
    // similar to just asserts
    precondition(!maps_.empty());
    maps_.pop_back();

    if (maps_.empty())
      scope_begin();
  }

  template <typename Key, typename Data>
  void scoped_map<Key, Data>::put(const Key& key, const Data& value)
  {
    maps_.back()[key] = value;
  }

  template <typename Key, typename Data>
  Data scoped_map<Key, Data>::get(const Key& key) const
  {
    for (int i = maps_.size() - 1; i >= 0; --i)
      {
        const map_type& curr_scope = maps_[i];
        if (curr_scope.count(key) > 0)
          return curr_scope.at(key);
      }

    // not found
    if (std::is_pointer<Data>::value)
      return static_cast<Data>(nullptr);
    else
      throw std::range_error("Key not found");
  }

  template <typename Key, typename Data>
  std::ostream& scoped_map<Key, Data>::dump(std::ostream& ostr) const
  {
    // i didn't find an example so just "human readable" but we choose
    for (int i = maps_.size() - 1; i >= 0; --i)
      {
        ostr << "Scope #nr" << (i + 1) << ":\n";

        for (const auto& pair : maps_[i])
          {
            ostr << " " << pair.first << " -> ";

            if constexpr (std::is_pointer<Data>::value)
            {
                if (pair.second)
                  ostr << *(pair.second);
                else
                  ostr << "nullptr";
              }
            else
              {
                ostr << pair.second;
              }
            ostr << '\n';
          }

        // newline except last one 
        if (i > 0)
          ostr << '\n';
      }

    return ostr;
  }

} // namespace misc