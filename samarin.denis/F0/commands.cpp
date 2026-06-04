#include "commands.hpp"

#include <cstddef>
#include <fstream>
#include <istream>
#include <limits>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <list.hpp>
#include "cuckoo_table.hpp"
#include "hashers.hpp"
#include "list_utils.hpp"
#include "stack.hpp"
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

    void requireStream(const std::istream& in)
    {
      if (!in) {
        throw std::logic_error("missing arguments");
      }
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
      LIter< std::string > tail = names.before_begin();
      for (Documents::const_iterator it = docs.cbegin(); it != docs.cend(); ++it) {
        tail = names.insert_after(tail, it->first);
      }
      listSort(names);
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
      requireStream(in);
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
      requireStream(in);
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

    using UnaryOp = Text (*)(const Text&);
    using BinaryOp = Text (*)(const Text&, const Text&);
    using RepeatOp = Text (*)(const Text&, std::size_t);

    void runUnary(std::istream& in, Documents& docs, UnaryOp op)
    {
      std::string id;
      std::string newId;
      in >> id >> newId;
      requireStream(in);
      requireAbsent(docs, newId);
      storeText(docs, newId, op(requireDoc(docs, id).restore()));
    }

    void runBinary(std::istream& in, Documents& docs, BinaryOp op)
    {
      std::string first;
      std::string second;
      std::string newId;
      in >> first >> second >> newId;
      requireStream(in);
      requireAbsent(docs, newId);
      const Text left = requireDoc(docs, first).restore();
      const Text right = requireDoc(docs, second).restore();
      storeText(docs, newId, op(left, right));
    }

    void runRepeat(std::istream& in, Documents& docs, RepeatOp op)
    {
      std::string id;
      std::size_t times = 0;
      std::string newId;
      in >> id >> times >> newId;
      requireStream(in);
      requireAbsent(docs, newId);
      storeText(docs, newId, op(requireDoc(docs, id).restore(), times));
    }

    void cmdConcat(std::istream& in, std::ostream&, Documents& docs)
    {
      runBinary(in, docs, &concat);
    }

    void cmdConcatLines(std::istream& in, std::ostream&, Documents& docs)
    {
      runBinary(in, docs, &concatLines);
    }

    void cmdInterleaveLines(std::istream& in, std::ostream&, Documents& docs)
    {
      runBinary(in, docs, &interleaveLines);
    }

    void cmdInterleaveWords(std::istream& in, std::ostream&, Documents& docs)
    {
      runBinary(in, docs, &interleaveWords);
    }

    void cmdRepeatVertical(std::istream& in, std::ostream&, Documents& docs)
    {
      runRepeat(in, docs, &repeatVertical);
    }

    void cmdRepeatHorizontal(std::istream& in, std::ostream&, Documents& docs)
    {
      runRepeat(in, docs, &repeatHorizontal);
    }

    void cmdReverseLines(std::istream& in, std::ostream&, Documents& docs)
    {
      runUnary(in, docs, &reverseLines);
    }

    void cmdReverseWords(std::istream& in, std::ostream&, Documents& docs)
    {
      runUnary(in, docs, &reverseWords);
    }

    void cmdTranspose(std::istream& in, std::ostream&, Documents& docs)
    {
      runUnary(in, docs, &transpose);
    }

    void cmdReplace(std::istream& in, std::ostream&, Documents& docs)
    {
      std::string id;
      std::size_t line = 0;
      std::size_t word = 0;
      std::string value;
      in >> id >> line >> word >> value;
      requireStream(in);
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
      requireStream(in);
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
      requireStream(in);
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
      requireStream(in);
      TextIndex& index = requireDoc(docs, id);
      Text text = index.restore();
      removeLine(text, toIndex(pos));
      index.build(text);
    }

    enum class Edge {
      both,
      left,
      right
    };

    struct findopts_t {
      std::size_t limit;
      bool fromEnd;
      Edge edge;
      std::size_t context;
    };

    bool startsWith(const std::string& text, const std::string& prefix)
    {
      return text.size() >= prefix.size() && text.compare(0, prefix.size(), prefix) == 0;
    }

    std::size_t parseSize(const std::string& text)
    {
      std::istringstream stream(text);
      std::size_t value = 0;
      if (!(stream >> value) || !stream.eof()) {
        throw std::logic_error("bad number");
      }
      return value;
    }

    void parseFlag(const std::string& flag, findopts_t& options)
    {
      if (startsWith(flag, "--limit=")) {
        options.limit = parseSize(flag.substr(std::string("--limit=").size()));
      } else if (startsWith(flag, "--context=")) {
        options.context = parseSize(flag.substr(std::string("--context=").size()));
      } else if (startsWith(flag, "--from=")) {
        const std::string value = flag.substr(std::string("--from=").size());
        if (value == "start") {
          options.fromEnd = false;
        } else if (value == "end") {
          options.fromEnd = true;
        } else {
          throw std::logic_error("bad from");
        }
      } else if (startsWith(flag, "--edge=")) {
        const std::string value = flag.substr(std::string("--edge=").size());
        if (value == "left") {
          options.edge = Edge::left;
        } else if (value == "right") {
          options.edge = Edge::right;
        } else {
          throw std::logic_error("bad edge");
        }
      } else {
        throw std::logic_error("unknown flag");
      }
    }

    List< position_t > collectSequence(const PostingList& source, bool fromEnd)
    {
      List< position_t > ordered = source;
      listSort(ordered);
      if (!fromEnd) {
        return ordered;
      }
      List< position_t > reversed;
      Stack< position_t > stack;
      for (LCIter< position_t > it = ordered.cbegin(); it != ordered.cend(); ++it) {
        stack.push(*it);
      }
      LIter< position_t > tail = reversed.before_begin();
      while (!stack.empty()) {
        tail = reversed.insert_after(tail, stack.drop());
      }
      return reversed;
    }

    void printContext(std::ostream& out, const Text& text, const position_t& pos, const findopts_t& options)
    {
      const Line& line = listAt(text, pos.line);
      const std::size_t width = listSize(line);
      std::size_t lo = pos.word;
      std::size_t hi = pos.word;
      if (options.edge != Edge::right) {
        lo = (pos.word > options.context) ? (pos.word - options.context) : 0;
      }
      if (options.edge != Edge::left) {
        hi = pos.word + options.context;
        if (hi >= width) {
          hi = width - 1;
        }
      }
      out << (pos.line + 1) << " " << (pos.word + 1) << ":";
      for (std::size_t col = lo; col <= hi; ++col) {
        const std::string& word = listAt(line, col);
        out << " ";
        if (col == pos.word) {
          out << "[" << word << "]";
        } else {
          out << word;
        }
      }
      out << "\n";
    }

    void cmdFind(std::istream& in, std::ostream& out, Documents& docs)
    {
      std::string id;
      std::string word;
      in >> id >> word;
      requireStream(in);
      findopts_t options = { std::numeric_limits< std::size_t >::max(), false, Edge::both, 0 };
      std::string flag;
      while (in >> flag) {
        parseFlag(flag, options);
      }
      const TextIndex& index = requireDoc(docs, id);
      if (!index.has(word)) {
        out << "<EMPTY>\n";
        return;
      }
      const List< position_t > sequence = collectSequence(index.positions(word), options.fromEnd);
      const Text text = index.restore();
      std::size_t shown = 0;
      for (LCIter< position_t > it = sequence.cbegin(); it != sequence.cend() && shown < options.limit; ++it) {
        printContext(out, text, *it, options);
        ++shown;
      }
    }

    void cmdDumpIndex(std::istream& in, std::ostream& out, Documents& docs)
    {
      std::string id;
      in >> id;
      const TextIndex& index = requireDoc(docs, id);
      List< std::string > words;
      LIter< std::string > wordTail = words.before_begin();
      for (PostingsTable::const_iterator it = index.postings().cbegin(); it != index.postings().cend(); ++it) {
        wordTail = words.insert_after(wordTail, it->first);
      }
      listSort(words);
      if (words.empty()) {
        out << "<EMPTY>\n";
        return;
      }
      for (LCIter< std::string > word = words.cbegin(); word != words.cend(); ++word) {
        out << *word << ":";
        List< position_t > positions = index.positions(*word);
        listSort(positions);
        for (LCIter< position_t > p = positions.cbegin(); p != positions.cend(); ++p) {
          out << " (" << (p->line + 1) << "," << (p->word + 1) << ")";
        }
        out << "\n";
      }
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
      commands.add("concat", &cmdConcat);
      commands.add("concat-lines", &cmdConcatLines);
      commands.add("repeat-vertical", &cmdRepeatVertical);
      commands.add("repeat-horizontal", &cmdRepeatHorizontal);
      commands.add("interleave-lines", &cmdInterleaveLines);
      commands.add("interleave-words", &cmdInterleaveWords);
      commands.add("reverse-lines", &cmdReverseLines);
      commands.add("reverse-words", &cmdReverseWords);
      commands.add("transpose", &cmdTranspose);
      commands.add("find", &cmdFind);
      commands.add("dump-index", &cmdDumpIndex);
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
