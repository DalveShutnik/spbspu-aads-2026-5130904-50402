#include <boost/test/unit_test.hpp>
#include <string>
#include <utility>
#include "hashers.hpp"

BOOST_AUTO_TEST_SUITE(hasher_tests)

BOOST_AUTO_TEST_CASE(string_hash_is_deterministic)
{
  const samarin::StringHash hasher;
  BOOST_TEST(hasher("vertex") == hasher("vertex"));
}

BOOST_AUTO_TEST_CASE(string_hash_separates_values)
{
  const samarin::StringHash hasher;
  BOOST_TEST(hasher("a") != hasher("b"));
  BOOST_TEST(hasher("ab") != hasher("ba"));
}

BOOST_AUTO_TEST_CASE(string_hash_is_case_sensitive)
{
  const samarin::StringHash hasher;
  BOOST_TEST(hasher("A") != hasher("a"));
}

BOOST_AUTO_TEST_CASE(pair_hash_is_order_sensitive)
{
  const samarin::PairHash hasher;
  const std::pair< std::string, std::string > direct("a", "b");
  const std::pair< std::string, std::string > reverse("b", "a");
  BOOST_TEST(hasher(direct) != hasher(reverse));
}

BOOST_AUTO_TEST_CASE(byte_hash_matches_string_hash)
{
  const std::string key = "weight";
  const samarin::StringHash hasher;
  BOOST_TEST(samarin::hashBytes(key.data(), key.size()) == hasher(key));
}

BOOST_AUTO_TEST_SUITE_END()
