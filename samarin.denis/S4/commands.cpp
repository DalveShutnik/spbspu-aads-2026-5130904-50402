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

  static CommandTable makeCommands()
  {
    CommandTable commands;
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
