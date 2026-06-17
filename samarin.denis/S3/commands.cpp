#include "commands.hpp"

#include <cstddef>
#include <istream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include "graph.hpp"
#include "hash_table.hpp"
#include "hashers.hpp"
#include "list.hpp"
#include "table_utils.hpp"

namespace samarin {

  using CommandHandler = void (*)(std::istream&, std::ostream&, GraphCollection&);
  using CommandTable = HashTable< std::string, CommandHandler, StringHash >;

  struct Neighbor {
    std::string name;
    Graph::WeightList weights;

    bool operator<(const Neighbor& other) const
    {
      return name < other.name;
    }
  };

  template< class T >
  static void sortedInsert(List< T >& list, const T& value)
  {
    LIter< T > prev = list.before_begin();
    LIter< T > cur = list.begin();
    while (cur != list.end() && *cur < value) {
      prev = cur;
      ++cur;
    }
    list.insert_after(prev, value);
  }

  template< class Table >
  static void printKeys(std::ostream& out, const Table& table)
  {
    List< std::string > names;
    for (auto it = table.cbegin(); it != table.cend(); ++it) {
      sortedInsert(names, it->first);
    }
    for (auto it = names.cbegin(); it != names.cend(); ++it) {
      out << *it << "\n";
    }
  }

  static void printEdges(std::ostream& out, const Graph& graph, const std::string& vertex, bool outbound)
  {
    if (!graph.hasVertex(vertex)) {
      throw std::logic_error("no such vertex");
    }
    List< Neighbor > neighbors;
    for (auto it = graph.edges().cbegin(); it != graph.edges().cend(); ++it) {
      const std::string& from = it->first.first;
      const std::string& to = it->first.second;
      const bool match = outbound ? (from == vertex) : (to == vertex);
      if (!match) {
        continue;
      }
      Neighbor entry;
      entry.name = outbound ? to : from;
      for (auto w = it->second.cbegin(); w != it->second.cend(); ++w) {
        sortedInsert(entry.weights, *w);
      }
      sortedInsert(neighbors, entry);
    }
    for (auto it = neighbors.cbegin(); it != neighbors.cend(); ++it) {
      out << it->name;
      for (auto w = it->weights.cbegin(); w != it->weights.cend(); ++w) {
        out << " " << *w;
      }
      out << "\n";
    }
  }

  static Graph& requireGraph(GraphCollection& graphs, const std::string& name)
  {
    if (!graphs.has(name)) {
      throw std::logic_error("no such graph");
    }
    return graphs.at(name);
  }

  static void cmdGraphs(std::istream&, std::ostream& out, GraphCollection& graphs)
  {
    printKeys(out, graphs);
  }

  static void cmdVertexes(std::istream& in, std::ostream& out, GraphCollection& graphs)
  {
    std::string name;
    in >> name;
    printKeys(out, requireGraph(graphs, name).vertices());
  }

  static void cmdOutbound(std::istream& in, std::ostream& out, GraphCollection& graphs)
  {
    std::string name;
    std::string vertex;
    in >> name >> vertex;
    printEdges(out, requireGraph(graphs, name), vertex, true);
  }

  static void cmdInbound(std::istream& in, std::ostream& out, GraphCollection& graphs)
  {
    std::string name;
    std::string vertex;
    in >> name >> vertex;
    printEdges(out, requireGraph(graphs, name), vertex, false);
  }

  static void cmdBind(std::istream& in, std::ostream&, GraphCollection& graphs)
  {
    std::string name;
    std::string from;
    std::string to;
    Graph::Weight weight = 0;
    in >> name >> from >> to >> weight;
    if (!in) {
      throw std::logic_error("invalid arguments");
    }
    requireGraph(graphs, name).bind(from, to, weight);
  }

  static void cmdCut(std::istream& in, std::ostream&, GraphCollection& graphs)
  {
    std::string name;
    std::string from;
    std::string to;
    Graph::Weight weight = 0;
    in >> name >> from >> to >> weight;
    if (!requireGraph(graphs, name).cut(from, to, weight)) {
      throw std::logic_error("no such edge");
    }
  }

  static void cmdCreate(std::istream& in, std::ostream&, GraphCollection& graphs)
  {
    std::string name;
    in >> name;
    if (graphs.has(name)) {
      throw std::logic_error("graph exists");
    }
    std::size_t count = 0;
    in >> count;
    Graph graph;
    for (std::size_t i = 0; i < count; ++i) {
      std::string vertex;
      if (!(in >> vertex)) {
        throw std::logic_error("missing vertex");
      }
      graph.addVertex(vertex);
    }
    insertOrGrow(graphs, name, graph);
  }

  static void cmdMerge(std::istream& in, std::ostream&, GraphCollection& graphs)
  {
    std::string newName;
    std::string first;
    std::string second;
    in >> newName >> first >> second;
    if (graphs.has(newName)) {
      throw std::logic_error("graph exists");
    }
    if (!graphs.has(first) || !graphs.has(second)) {
      throw std::logic_error("no such graph");
    }
    Graph graph;
    graph.absorb(graphs.at(first));
    graph.absorb(graphs.at(second));
    insertOrGrow(graphs, newName, graph);
  }

  static void cmdExtract(std::istream& in, std::ostream&, GraphCollection& graphs)
  {
    std::string newName;
    std::string oldName;
    in >> newName >> oldName;
    if (graphs.has(newName)) {
      throw std::logic_error("graph exists");
    }
    if (!graphs.has(oldName)) {
      throw std::logic_error("no such graph");
    }
    std::size_t count = 0;
    if (!(in >> count)) {
      throw std::logic_error("missing count");
    }
    const Graph& source = graphs.at(oldName);
    Graph graph;
    for (std::size_t i = 0; i < count; ++i) {
      std::string vertex;
      if (!(in >> vertex)) {
        throw std::logic_error("missing vertex");
      }
      if (!source.hasVertex(vertex)) {
        throw std::logic_error("no such vertex");
      }
      graph.addVertex(vertex);
    }
    graph.copyEdgesWithin(source);
    insertOrGrow(graphs, newName, graph);
  }

  static CommandTable makeCommands()
  {
    CommandTable commands;
    commands.add("graphs", &cmdGraphs);
    commands.add("vertexes", &cmdVertexes);
    commands.add("outbound", &cmdOutbound);
    commands.add("inbound", &cmdInbound);
    commands.add("bind", &cmdBind);
    commands.add("cut", &cmdCut);
    commands.add("create", &cmdCreate);
    commands.add("merge", &cmdMerge);
    commands.add("extract", &cmdExtract);
    return commands;
  }

}

void samarin::processCommands(std::istream& in, std::ostream& out, GraphCollection& graphs)
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
      const CommandTable::const_iterator handler = commands.find(name);
      if (handler == commands.cend()) {
        throw std::logic_error("unknown command");
      }
      handler->second(stream, out, graphs);
    } catch (const std::exception&) {
      out << "<INVALID COMMAND>\n";
    }
  }
}
