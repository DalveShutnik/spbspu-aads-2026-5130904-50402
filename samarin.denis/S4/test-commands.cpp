#include <boost/test/unit_test.hpp>
#include <sstream>
#include <string>
#include "commands.hpp"
#include "dataset.hpp"
#include "input.hpp"

namespace {
  samarin::DatasetCollection load(const std::string & text)
  {
    std::istringstream in(text);
    samarin::DatasetCollection datasets;
    samarin::readDatasets(in, datasets);
    return datasets;
  }

  std::string run(samarin::DatasetCollection & datasets, const std::string & commands)
  {
    std::istringstream in(commands);
    std::ostringstream out;
    samarin::processCommands(in, out, datasets);
    return out.str();
  }
}

BOOST_AUTO_TEST_SUITE(command_tests)

BOOST_AUTO_TEST_CASE(print_sorts_by_key)
{
  samarin::DatasetCollection datasets = load("second 4 mouse 1 name 2 keyboard\n");
  BOOST_TEST(run(datasets, "print second\n") == "second 1 name 2 keyboard 4 mouse\n");
}

BOOST_AUTO_TEST_CASE(print_empty_reports_marker)
{
  samarin::DatasetCollection datasets = load("empty\n");
  BOOST_TEST(run(datasets, "print empty\n") == "<EMPTY>\n");
}

BOOST_AUTO_TEST_CASE(unknown_command_is_invalid)
{
  samarin::DatasetCollection datasets = load("a 1 x\n");
  BOOST_TEST(run(datasets, "frobnicate a\n") == "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(missing_dataset_is_invalid)
{
  samarin::DatasetCollection datasets = load("a 1 x\n");
  BOOST_TEST(run(datasets, "print b\n") == "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_SUITE_END()
