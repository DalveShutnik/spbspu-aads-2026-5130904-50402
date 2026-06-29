#include <boost/test/unit_test.hpp>
#include <stdexcept>
#include <string>
#include "tree.hpp"

namespace {
  using Tree = samarin::BSTree< int, std::string >;
}

BOOST_AUTO_TEST_SUITE(tree_tests)

BOOST_AUTO_TEST_CASE(push_and_get)
{
  Tree tree;
  tree.push(2, "two");
  tree.push(1, "one");
  tree.push(3, "three");
  BOOST_TEST(tree.size() == 3u);
  BOOST_TEST(tree.get(1) == "one");
  BOOST_TEST(tree.get(2) == "two");
  BOOST_TEST(tree.get(3) == "three");
}

BOOST_AUTO_TEST_CASE(push_overwrites_value)
{
  Tree tree;
  tree.push(1, "old");
  tree.push(1, "new");
  BOOST_TEST(tree.size() == 1u);
  BOOST_TEST(tree.get(1) == "new");
}

BOOST_AUTO_TEST_CASE(get_missing_throws)
{
  Tree tree;
  BOOST_CHECK_THROW(tree.get(7), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(iteration_is_sorted)
{
  Tree tree;
  tree.push(5, "e");
  tree.push(1, "a");
  tree.push(4, "d");
  tree.push(2, "b");
  tree.push(3, "c");
  std::string keys;
  for (Tree::const_iterator it = tree.cbegin(); it != tree.cend(); ++it) {
    keys += it->second;
  }
  BOOST_TEST(keys == "abcde");
}

BOOST_AUTO_TEST_CASE(contains_reports_membership)
{
  Tree tree;
  tree.push(10, "x");
  BOOST_TEST(tree.contains(10));
  BOOST_TEST(!tree.contains(11));
}

BOOST_AUTO_TEST_SUITE_END()
