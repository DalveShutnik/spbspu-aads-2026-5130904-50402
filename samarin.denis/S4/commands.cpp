#include "commands.hpp"

#include <istream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include "dataset.hpp"
#include "tree.hpp"

namespace samarin {

  using CommandHandler = void (*)(std::istream &, std::ostream &, DatasetCollection &);
  using CommandTable = BSTree< std::string, CommandHandler >;

  static Dataset & requireDataset(DatasetCollection & datasets, const std::string & name)
  {
    DatasetCollection::iterator it = datasets.find(name);
    if (it == datasets.end()) {
      throw std::logic_error("no such dataset");
    }
    return it->second;
  }

  static void cmdPrint(std::istream & in, std::ostream & out, DatasetCollection & datasets)
  {
    std::string name;
    if (!(in >> name)) {
      throw std::logic_error("missing name");
    }
    const Dataset & dataset = requireDataset(datasets, name);
    if (dataset.empty()) {
      out << "<EMPTY>\n";
      return;
    }
    out << name;
    for (Dataset::const_iterator it = dataset.cbegin(); it != dataset.cend(); ++it) {
      out << " " << it->first << " " << it->second;
    }
    out << "\n";
  }

  static CommandTable makeCommands()
  {
    CommandTable commands;
    commands.push("print", &cmdPrint);
    return commands;
  }

}

void samarin::processCommands(std::istream & in, std::ostream & out, DatasetCollection & datasets)
{
  const CommandTable commands = makeCommands();
  std::string line;
  while (std::getline(in, line)) {
    std::istringstream stream(line);
    std::string name;
    if (!(stream >> name)) {
      continue;
    }
    try {
      CommandTable::const_iterator handler = commands.find(name);
      if (handler == commands.cend()) {
        throw std::logic_error("unknown command");
      }
      handler->second(stream, out, datasets);
    } catch (const std::exception &) {
      out << "<INVALID COMMAND>\n";
    }
  }
}
