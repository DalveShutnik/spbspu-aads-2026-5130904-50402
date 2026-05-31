#ifndef CALCULATOR_HPP
#define CALCULATOR_HPP

#include <string>
#include "queue.hpp"

namespace samarin {

  Queue< std::string > convertToPostfix(Queue< std::string > infix);

}

#endif
