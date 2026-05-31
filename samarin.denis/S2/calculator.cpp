#include "calculator.hpp"

#include <stdexcept>
#include <string>
#include "stack.hpp"

namespace samarin {

  static bool isOperator(const std::string& token)
  {
    return token == "+" || token == "-" || token == "*"
      || token == "/" || token == "%";
  }

  static int precedence(const std::string& op)
  {
    if (op == "*" || op == "/" || op == "%") {
      return 2;
    }
    return 1;
  }

}

samarin::Queue< std::string > samarin::convertToPostfix(Queue< std::string > infix)
{
  Queue< std::string > output;
  Stack< std::string > operators;
  while (!infix.empty()) {
    const std::string token = infix.drop();
    if (token == "(") {
      operators.push(token);
    } else if (token == ")") {
      while (!operators.empty() && operators.top() != "(") {
        output.push(operators.drop());
      }
      if (operators.empty()) {
        throw std::logic_error("mismatched parentheses");
      }
      operators.drop();
    } else if (isOperator(token)) {
      while (!operators.empty() && operators.top() != "("
          && precedence(operators.top()) >= precedence(token)) {
        output.push(operators.drop());
      }
      operators.push(token);
    } else {
      output.push(token);
    }
  }
  while (!operators.empty()) {
    if (operators.top() == "(") {
      throw std::logic_error("mismatched parentheses");
    }
    output.push(operators.drop());
  }
  return output;
}
