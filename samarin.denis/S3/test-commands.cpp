#include <boost/test/unit_test.hpp>
#include <sstream>
#include <string>
#include "commands.hpp"
#include "graph_collection.hpp"
#include "input.hpp"

namespace {
  std::string run(const std::string& data, const std::string& script)
  {
    std::istringstream file(data);
    samarin::GraphCollection graphs;
    samarin::readGraphs(file, graphs);
    std::istringstream commands(script);
    std::ostringstream out;
    samarin::processCommands(commands, out, graphs);
    return out.str();
  }

  const char* const sample = "gr1 4\na b 40\nb c 50\nc a 30\nc b 20\n"
      "gr2 3\na b 1\nb b 2\na c 3\n5\n";
}

BOOST_AUTO_TEST_SUITE(command_tests)

BOOST_AUTO_TEST_CASE(graphs_lists_sorted)
{
  BOOST_TEST(run(sample, "graphs\n") == "gr1\ngr2\n");
}

BOOST_AUTO_TEST_CASE(vertexes_lists_and_rejects_missing)
{
  BOOST_TEST(run(sample, "vertexes gr1\nvertexes gr3\n") == "a\nb\nc\n<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(outbound_reports_sorted_weights)
{
  BOOST_TEST(run(sample, "outbound gr1 c\n") == "a 30\nb 20\n");
}

BOOST_AUTO_TEST_CASE(inbound_reports_sorted_weights)
{
  BOOST_TEST(run(sample, "inbound gr1 b\n") == "a 40\nc 20\n");
}

BOOST_AUTO_TEST_CASE(bind_then_outbound)
{
  BOOST_TEST(run(sample, "bind gr2 b d 200\noutbound gr2 b\n") == "b 2\nd 200\n");
}

BOOST_AUTO_TEST_CASE(cut_removes_edge)
{
  BOOST_TEST(run(sample, "cut gr2 b b 1\ncut gr2 b b 2\ninbound gr2 b\n") == "<INVALID COMMAND>\na 1\n");
}

BOOST_AUTO_TEST_CASE(create_rejects_existing)
{
  BOOST_TEST(run(sample, "create gr3\ncreate gr3\ngraphs\n") == "<INVALID COMMAND>\ngr1\ngr2\ngr3\n");
}

BOOST_AUTO_TEST_CASE(merge_concatenates_weights)
{
  BOOST_TEST(run(sample, "merge gr3 gr2 gr2\ninbound gr3 b\n") == "a 1 1\nb 2 2\n");
}

BOOST_AUTO_TEST_CASE(extract_keeps_inner_edges)
{
  BOOST_TEST(run(sample, "extract gr3 gr2 2 a c\noutbound gr3 a\n") == "c 3\n");
}

BOOST_AUTO_TEST_CASE(unknown_command_is_invalid)
{
  BOOST_TEST(run(sample, "nonsense\n") == "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_SUITE_END()
