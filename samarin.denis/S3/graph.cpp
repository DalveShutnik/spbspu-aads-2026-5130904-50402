#include "graph.hpp"

bool samarin::Graph::hasVertex(const std::string& name) const
{
  return vertices_.has(name);
}

void samarin::Graph::addVertex(const std::string& name)
{
  if (vertices_.has(name)) {
    return;
  }
  while (true) {
    try {
      vertices_.add(name, true);
      return;
    } catch (const std::length_error&) {
      vertices_.rehash(vertices_.capacity() * 2);
    }
  }
}

void samarin::Graph::bind(const std::string& from, const std::string& to, Weight weight)
{
  addVertex(from);
  addVertex(to);
  const EdgeKey key(from, to);
  while (true) {
    try {
      edges_[key].push_front(weight);
      return;
    } catch (const std::length_error&) {
      edges_.rehash(edges_.capacity() * 2);
    }
  }
}

bool samarin::Graph::cut(const std::string& from, const std::string& to, Weight weight)
{
  const EdgeKey key(from, to);
  if (!edges_.has(key)) {
    return false;
  }
  WeightList& weights = edges_.at(key);
  LIter< Weight > prev = weights.before_begin();
  for (LIter< Weight > cur = weights.begin(); cur != weights.end(); ++cur) {
    if (*cur == weight) {
      weights.erase_after(prev);
      if (weights.empty()) {
        edges_.drop(key);
      }
      return true;
    }
    prev = cur;
  }
  return false;
}

void samarin::Graph::absorb(const Graph& other)
{
  for (auto it = other.vertices_.cbegin(); it != other.vertices_.cend(); ++it) {
    addVertex(it->first);
  }
  for (auto it = other.edges_.cbegin(); it != other.edges_.cend(); ++it) {
    for (auto w = it->second.cbegin(); w != it->second.cend(); ++w) {
      bind(it->first.first, it->first.second, *w);
    }
  }
}

const samarin::Graph::VertexTable& samarin::Graph::vertices() const
{
  return vertices_;
}

const samarin::Graph::EdgeTable& samarin::Graph::edges() const
{
  return edges_;
}
