#include "regex.h"
#include "graph.h"

namespace automata
{
class exGraph : virtual public Graph
{
  public:
    std::map<int, std::string> labels;
    int get_id(std::string lbl);
    bool set_lbl(int id, std::string lbl);

    exGraph() : Graph()
    {
        labels[0] = "start";
        nodes.insert(0);
    }
    int add(std::string lbl);
    int add_or_get(std::string lbl);
    void add_edge(std::string f, std::string t, char c);

    using Graph::add;
    using subgraph::add_edge;

    struct status;

    status operator[](std::string lbl);
    friend std::ostream &operator<<(std::ostream &os, exGraph &g);
};

struct exGraph::status
{
    exGraph &parent;
    int id;

    class hdl;
    hdl operator[](char c);
    std::string operator=(std::string lbl);
    int operator=(int ac);
};

struct exGraph::status::hdl
{
    exGraph::status &s;
    char c;

    std::string operator=(std::string t_lbl);
};

class exNFA : public NFAGraph, public exGraph
{
  public:
    exNFA() : NFAGraph(), exGraph() {}
    exNFA(NFAGraph &g) : NFAGraph(g), exGraph() {}
};

} // namespace automata