#include <boost/test/unit_test.hpp>
#include <memory>
#include <list.hpp>

namespace {
  struct Point {
    int x;
    int y;
    Point(int a, int b):
      x(a),
      y(b)
    {}
  };
}

BOOST_AUTO_TEST_SUITE(emplace_tests)

BOOST_AUTO_TEST_CASE(emplace_front_constructs_in_place)
{
  samarin::List< Point > lst;
  lst.emplaceFront(1, 2);
  BOOST_TEST(lst.front().x == 1);
  BOOST_TEST(lst.front().y == 2);
}

BOOST_AUTO_TEST_CASE(emplace_after_constructs_in_place)
{
  samarin::List< Point > lst;
  samarin::LIter< Point > tail = lst.before_begin();
  tail = lst.emplaceAfter(tail, 3, 4);
  BOOST_TEST(lst.front().x == 3);
  BOOST_TEST(lst.front().y == 4);
}

BOOST_AUTO_TEST_CASE(emplace_forwards_move_only)
{
  samarin::List< std::unique_ptr< int > > lst;
  lst.emplaceFront(new int(42));
  BOOST_TEST(*lst.front() == 42);
}

BOOST_AUTO_TEST_SUITE_END()
