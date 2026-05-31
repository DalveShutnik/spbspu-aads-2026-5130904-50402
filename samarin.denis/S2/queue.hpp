#ifndef QUEUE_HPP
#define QUEUE_HPP

#include "list.hpp"

namespace samarin {

  template< class T >
  class Queue {
  public:
    void push(const T& rhs);
    T drop();
    bool empty() const;

  private:
    List< T > data_;
  };

}

#endif
