#include "commands.hpp"

#include <cstddef>
#include <fstream>
#include <istream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <list.hpp>
#include "cuckoo_table.hpp"
#include "hashers.hpp"
#include "list_utils.hpp"
#include "text.hpp"
#include "text_index.hpp"

namespace samarin {

  using CommandHandler = void (*)(std::istream&, std::ostream&, Documents&);
  using CommandTable = CuckooTable< std::string, CommandHandler, StringHash >;

  namespace {
    TextIndex& requireDoc(Documents& docs, const std::string& id)
    {
      if (!docs.has(id)) {
        throw std::logic_error("no such document");
      }
      return docs.at(id);
    }

    void requireAbsent(const Documents& docs, const std::string& id)
    {
      if (docs.has(id)) {
        throw std::logic_error("document exists");
      }
    }

    void storeText(Documents& docs, const std::string& id, const Text& text)
    {
      TextIndex index;
      index.build(text);
      docs.add(id, index);
    }

    void printKeys(std::ostream& out, const Documents& docs)
    {
      List< std::string > names;
      for (Documents::const_iterator it = docs.cbegin(); it != docs.cend(); ++it) {
        sortedInsert(names, it->first);
      }
      if (names.empty()) {
        out << "<EMPTY>\n";
        return;
      }
      for (LCIter< std::string > it = names.cbegin(); it != names.cend(); ++it) {
        out << *it << "\n";
      }
    }

    void cmdLoad(std::istream& in, std::ostream&, Documents& docs)
    {
      std::string file;
      std::string id;
      in >> file >> id;
      if (!in) {
        throw std::logic_error("missing arguments");
      }
      requireAbsent(docs, id);
      std::ifstream source(file.c_str());
      if (!source.is_open()) {
        throw std::logic_error("cannot open file");
      }
      storeText(docs, id, parseText(source));
    }

    void cmdSave(std::istream& in, std::ostream&, Documents& docs)
    {
      std::string id;
      std::string file;
      in >> id >> file;
      if (!in) {
        throw std::logic_error("missing arguments");
      }
      const TextIndex& index = requireDoc(docs, id);
      std::ofstream sink(file.c_str());
      if (!sink.is_open()) {
        throw std::logic_error("cannot open file");
      }
      writeText(sink, index.restore());
    }

    void cmdList(std::istream&, std::ostream& out, Documents& docs)
    {
      printKeys(out, docs);
    }

    void cmdDrop(std::istream& in, std::ostream&, Documents& docs)
    {
      std::string id;
      in >> id;
      requireDoc(docs, id);
      docs.drop(id);
    }

    void cmdShow(std::istream& in, std::ostream& out, Documents& docs)
    {
      std::string id;
      in >> id;
      writeText(out, requireDoc(docs, id).restore());
    }

    void cmdHelp(std::istream&, std::ostream& out, Documents&)
    {
      out << "load save list drop show help exit\n";
      out << "replace swap insert-line remove-line\n";
      out << "concat concat-lines repeat-vertical repeat-horizontal\n";
      out << "interleave-lines interleave-words reverse-lines reverse-words transpose\n";
      out << "find dump-index\n";
    }

    CommandTable makeCommands()
    {
      CommandTable commands;
      commands.add("load", &cmdLoad);
      commands.add("save", &cmdSave);
      commands.add("list", &cmdList);
      commands.add("drop", &cmdDrop);
      commands.add("show", &cmdShow);
      commands.add("help", &cmdHelp);
      return commands;
    }
  }

}

void samarin::processCommands(std::istream& in, std::ostream& out, Documents& docs)
{
  const CommandTable commands = makeCommands();
  std::string line;
  while (std::getline(in, line)) {
    std::istringstream stream(line);
    std::string name;
    if (!(stream >> name)) {
      continue;
    }
    if (name == "exit") {
      break;
    }
    try {
      const CommandTable::const_iterator handler = commands.find(name);
      if (handler == commands.cend()) {
        throw std::logic_error("unknown command");
      }
      handler->second(stream, out, docs);
    } catch (const std::exception&) {
      out << "<INVALID COMMAND>\n";
    }
  }
}
