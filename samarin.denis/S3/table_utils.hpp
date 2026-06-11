#ifndef TABLE_UTILS_HPP
#define TABLE_UTILS_HPP

#include <stdexcept>

namespace samarin {

  template< class Table, class Key, class Value >
  void insertOrGrow(Table& table, const Key& key, const Value& value)
  {
    while (true) {
      try {
        table.add(key, value);
        return;
      } catch (const std::length_error&) {
        table.rehash(table.capacity() * 2);
      }
    }
  }

}

#endif
