//#pragma once
#ifndef __Graph_H__
#define __Graph_H__

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include "regex.h"

namespace automata
{
class Graph;
class NFAGraph;
class DFAGraph;

using node = int;
using node_set = std::set<node>;
using init_method = int;

const std::string accepting = "GRAPH_ACCEPTING";
const std::string start = "GRAPH_STARTING";

constexpr init_method copy = 0;
constexpr init_method conv = 1;
// constexpr init_method copy_and_check = 2;

class Graph : public subgraph
{
protected:
  std::map<int, std::string> labels;

  node_set nodes, tos;
  node_set get_closure(node_set &s);

public:
  friend class NFAGraph;
  friend class DFAGraph;
  Graph();
  Graph(const Graph &g) = default;
  Graph(const subgraph &sg);

  node add();
  node add(std::string lbl);
  node get(std::string lbl);
  node add_or_get(std::string lbl);
  node set_lbl(int id, std::string lbl);
  std::string get(node id);

  using subgraph::add_edge;
  void add_edge(std::string f, std::string t, char c);

  void export_dot(std::string filename);
  friend std::ostream &operator<<(std::ostream &os, Graph &nfa);

  struct status;
  status operator[](std::string lbl);
};

class NFAGraph : virtual public Graph
{
public:
  NFAGraph() : Graph() {}
  NFAGraph(const NFAGraph &g) = default;
  NFAGraph(const Graph &g) : Graph(g) {}
  NFAGraph(const RNDRegex &r) : Graph(RNDRegex(r).reg2nfa()) {}
  NFAGraph(std::string s) : Graph(RNDRegex(s).reg2nfa()) {}

  Graph nfa2dfa();
};

class DFAGraph : virtual public Graph
{
public:
  DFAGraph() : Graph() {}
  DFAGraph(const DFAGraph &g) = default;
  DFAGraph(const Graph &g, init_method m = automata::conv)
      : Graph(m == automata::copy ? g : NFAGraph(g).nfa2dfa()) {}
  DFAGraph(const RNDRegex &r) : Graph(NFAGraph(r).nfa2dfa()) {}
  DFAGraph(std::string s) : Graph(NFAGraph(s).nfa2dfa()) {}

  bool check();
  bool match(std::string str);
};

struct Graph::status
{
  Graph &parent;
  int id;

  class hdl;
  hdl operator[](char c);
  std::string operator=(std::string lbl);
};

struct Graph::status::hdl
{
  Graph::status &s;
  char c;

  std::string operator=(std::string t_lbl);
};

} // namespace automata

#endif