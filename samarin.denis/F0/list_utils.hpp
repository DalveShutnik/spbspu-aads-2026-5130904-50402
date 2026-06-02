#ifndef LIST_UTILS_HPP
#define LIST_UTILS_HPP

#include <cstddef>
#include <list.hpp>

namespace samarin {

  template< class T >
  std::size_t listSize(const List< T >& list)
  {
    std::size_t count = 0;
    for (LCIter< T > it = list.cbegin(); it != list.cend(); ++it) {
      ++count;
    }
    return count;
  }

  template< class T >
  void sortedInsert(List< T >& list, const T& value)
  {
    LIter< T > prev = list.before_begin();
    LIter< T > cur = list.begin();
    while (cur != list.end() && *cur < value) {
      prev = cur;
      ++cur;
    }
    list.insert_after(prev, value);
  }

}

#endif
