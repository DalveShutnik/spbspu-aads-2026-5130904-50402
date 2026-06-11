#ifndef HASHERS_HPP
#define HASHERS_HPP

#include <cstddef>
#include <string>
#include <utility>

namespace samarin {

  size_t hashBytes(const char* data, size_t size);

  struct StringHash {
    size_t operator()(const std::string& key) const;
  };

  struct PairHash {
    size_t operator()(const std::pair< std::string, std::string >& key) const;
  };

}

#endif
