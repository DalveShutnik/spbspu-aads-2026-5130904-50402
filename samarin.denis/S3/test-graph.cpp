#include <boost/test/unit_test.hpp>
#include <string>
#include "graph.hpp"

namespace {
  size_t countWeights(const samarin::Graph& graph, const std::string& from, const std::string& to)
  {
    const samarin::Graph::EdgeKey key(from, to);
    if (!graph.edges().has(key)) {
      return 0;
    }
    size_t count = 0;
    const samarin::Graph::WeightList& weights = graph.edges().at(key);
    for (auto it = weights.cbegin(); it != weights.cend(); ++it) {
      ++count;
    }
    return count;
  }
}

BOOST_AUTO_TEST_SUITE(graph_tests)

BOOST_AUTO_TEST_CASE(bind_adds_vertices_and_edge)
{
  samarin::Graph graph;
  graph.bind("a", "b", 40);
  BOOST_TEST(graph.hasVertex("a"));
  BOOST_TEST(graph.hasVertex("b"));
  BOOST_TEST(countWeights(graph, "a", "b") == 1u);
}

BOOST_AUTO_TEST_CASE(bind_accumulates_weights)
{
  samarin::Graph graph;
  graph.bind("a", "b", 1);
  graph.bind("a", "b", 2);
  BOOST_TEST(countWeights(graph, "a", "b") == 2u);
}

BOOST_AUTO_TEST_CASE(cut_removes_one_weight)
{
  samarin::Graph graph;
  graph.bind("a", "b", 1);
  graph.bind("a", "b", 1);
  BOOST_TEST(graph.cut("a", "b", 1));
  BOOST_TEST(countWeights(graph, "a", "b") == 1u);
}

BOOST_AUTO_TEST_CASE(cut_missing_returns_false)
{
  samarin::Graph graph;
  graph.bind("a", "b", 1);
  BOOST_TEST(!graph.cut("a", "b", 9));
  BOOST_TEST(!graph.cut("x", "y", 1));
}

BOOST_AUTO_TEST_CASE(cut_last_weight_drops_edge)
{
  samarin::Graph graph;
  graph.bind("a", "b", 5);
  BOOST_TEST(graph.cut("a", "b", 5));
  BOOST_TEST(countWeights(graph, "a", "b") == 0u);
}

BOOST_AUTO_TEST_CASE(absorb_duplicates_weights)
{
  samarin::Graph source;
  source.bind("a", "b", 1);
  source.bind("b", "b", 2);
  samarin::Graph merged;
  merged.absorb(source);
  merged.absorb(source);
  BOOST_TEST(countWeights(merged, "a", "b") == 2u);
  BOOST_TEST(countWeights(merged, "b", "b") == 2u);
}

BOOST_AUTO_TEST_SUITE_END()
