#ifndef STACK_HPP
#define STACK_HPP

#include "list.hpp"

namespace samarin {

  template< class T >
  class Stack {
  public:
    void push(const T& rhs);
    T drop();
    bool empty() const;

  private:
    List< T > data_;
  };

}

#endif
