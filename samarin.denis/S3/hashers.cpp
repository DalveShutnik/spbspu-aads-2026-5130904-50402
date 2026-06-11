#include "hashers.hpp"

size_t samarin::hashBytes(const char* data, size_t size)
{
  const size_t offset = 14695981039346656037ull;
  const size_t prime = 1099511628211ull;
  size_t hash = offset;
  for (size_t i = 0; i < size; ++i) {
    hash ^= static_cast< unsigned char >(data[i]);
    hash *= prime;
  }
  return hash;
}

size_t samarin::StringHash::operator()(const std::string& key) const
{
  return hashBytes(key.data(), key.size());
}

size_t samarin::PairHash::operator()(const std::pair< std::string, std::string >& key) const
{
  const StringHash hasher;
  const size_t first = hasher(key.first);
  const size_t second = hasher(key.second);
  const size_t golden = 0x9e3779b97f4a7c15ull;
  return first ^ (second + golden + (first << 6) + (first >> 2));
}
