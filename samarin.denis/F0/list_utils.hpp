#ifndef LIST_UTILS_HPP
#define LIST_UTILS_HPP

#include <cstddef>
#include <stdexcept>
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
  T& listAt(List< T >& list, std::size_t index)
  {
    LIter< T > it = list.begin();
    for (std::size_t i = 0; i < index; ++i) {
      if (it == list.end()) {
        throw std::out_of_range("index out of range");
      }
      ++it;
    }
    if (it == list.end()) {
      throw std::out_of_range("index out of range");
    }
    return *it;
  }

  template< class T >
  LIter< T > iterBefore(List< T >& list, std::size_t index)
  {
    LIter< T > prev = list.before_begin();
    for (std::size_t i = 0; i < index; ++i) {
      LIter< T > next = prev;
      ++next;
      if (next == list.end()) {
        throw std::out_of_range("index out of range");
      }
      prev = next;
    }
    return prev;
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
