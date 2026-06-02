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
#include "text_ops.hpp"

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

    std::size_t toIndex(std::size_t oneBased)
    {
      if (oneBased == 0) {
        throw std::logic_error("index must be positive");
      }
      return oneBased - 1;
    }

    Line readLine(std::istream& in)
    {
      Line line;
      LIter< std::string > tail = line.before_begin();
      std::string word;
      while (in >> word) {
        tail = line.insert_after(tail, word);
      }
      return line;
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

    void cmdReplace(std::istream& in, std::ostream&, Documents& docs)
    {
      std::string id;
      std::size_t line = 0;
      std::size_t word = 0;
      std::string value;
      in >> id >> line >> word >> value;
      if (!in) {
        throw std::logic_error("missing arguments");
      }
      TextIndex& index = requireDoc(docs, id);
      Text text = index.restore();
      replaceWord(text, toIndex(line), toIndex(word), value);
      index.build(text);
    }

    void cmdSwap(std::istream& in, std::ostream&, Documents& docs)
    {
      std::string id;
      std::size_t line1 = 0;
      std::size_t word1 = 0;
      std::size_t line2 = 0;
      std::size_t word2 = 0;
      in >> id >> line1 >> word1 >> line2 >> word2;
      if (!in) {
        throw std::logic_error("missing arguments");
      }
      TextIndex& index = requireDoc(docs, id);
      Text text = index.restore();
      swapWords(text, toIndex(line1), toIndex(word1), toIndex(line2), toIndex(word2));
      index.build(text);
    }

    void cmdInsertLine(std::istream& in, std::ostream&, Documents& docs)
    {
      std::string id;
      std::size_t pos = 0;
      in >> id >> pos;
      if (!in) {
        throw std::logic_error("missing arguments");
      }
      TextIndex& index = requireDoc(docs, id);
      const Line line = readLine(in);
      Text text = index.restore();
      insertLine(text, toIndex(pos), line);
      index.build(text);
    }

    void cmdRemoveLine(std::istream& in, std::ostream&, Documents& docs)
    {
      std::string id;
      std::size_t pos = 0;
      in >> id >> pos;
      if (!in) {
        throw std::logic_error("missing arguments");
      }
      TextIndex& index = requireDoc(docs, id);
      Text text = index.restore();
      removeLine(text, toIndex(pos));
      index.build(text);
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
      commands.add("replace", &cmdReplace);
      commands.add("swap", &cmdSwap);
      commands.add("insert-line", &cmdInsertLine);
      commands.add("remove-line", &cmdRemoveLine);
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
