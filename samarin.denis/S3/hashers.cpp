#include "hashers.hpp"

#include <boost/hash2/hash_append.hpp>
#include <boost/hash2/siphash.hpp>

std::size_t samarin::StringHash::operator()(const std::string& key) const
{
  boost::hash2::siphash_64 hasher;
  boost::hash2::hash_append(hasher, {}, key);
  return static_cast< std::size_t >(hasher.result());
}

std::size_t samarin::PairHash::operator()(const std::pair< std::string, std::string >& key) const
{
  const StringHash hasher;
  const std::size_t first = hasher(key.first);
  const std::size_t second = hasher(key.second);
  const std::size_t golden = 0x9e3779b97f4a7c15ull;
  return first ^ (second + golden + (first << 6) + (first >> 2));
}
