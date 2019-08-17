#include <queue>
#include <bitset>
#include "graph.h"
#define MAX_SIZE 64

namespace automata
{
inline std::string itos(int n)
{
    char buffer[3];
    sprintf(buffer, "%d", n);
    return std::string(buffer);
}

Graph::Graph()
{
    from = 0;
}

Graph::Graph(const subgraph &sg)
{
    for (auto &i : sg.trans)
    {
        nodes.insert(i.first);
    }
    nodes.insert(sg.to);
    from = sg.from;
    tos.insert(sg.to);
    trans = sg.trans;
}

node Graph::add()
{
    int id = nodes.size();
    nodes.insert(id);
    return id;
}

node Graph::add(std::string lbl)
{
    node id = add();
    labels[id] = lbl;
    return id;
}

node Graph::get(std::string lbl)
{
    for (auto i : labels)
        if (i.second == lbl)
            return i.first;
    return -1;
}

node Graph::add_or_get(std::string lbl)
{
    int id = get(lbl);
    if (id < 0)
        id = add(lbl);
    return id;
}

node Graph::set_lbl(int id, std::string lbl)
{
    if (nodes.find(id) == nodes.end())
        return -1;
    labels[id] = lbl;
    return id;
}

std::string Graph::get(node id)
{
    if (labels.count(id) == 0)
        return itos(id);
    else
        return labels[id];
}

void Graph::add_edge(std::string f, std::string t, char c = '\0')
{
    int f_id = get(f);
    int t_id = get(t);
    add_edge(f_id, t_id, c);
}

std::ostream &operator<<(std::ostream &os, Graph &g)
{
    os << "sum: " << g.nodes.size() << std::endl;
    for (auto &id : g.nodes)
    {
        os << id << "[";
        os << g.get(id) << "]: ";
        if (g.trans.count(id) == 0)
        {
            os << std::endl;
            continue;
        }

        for (auto &_c : g.trans[id])
        {
            for (auto &_edge : _c.second)
            {
                os << "[" << _c.first
                   << ":" << _edge << "] ";
            }
        }
        os << std::endl;
    }
    os << g.from << " -> ";
    for (auto &i : g.tos)
        os << i << " ";
    os << std::endl;
    return os;
}

void Graph::export_dot(std::string filename)
{
    std::ofstream os(filename);
    os << "digraph nfa {" << std::endl
       << "\trankdir = LR;" << std::endl
       << std::endl
       << "\tSt[label=\"\",color=white];"
       << std::endl;
    for (auto &n : nodes)
    {
        os << "\t_" << n << "[label=\""
           << get(n) << "\",shape=";
        if (tos.find(n) != tos.end())
            os << "doublecircle";
        else
            os << "circle";
        os << "];" << std::endl;
    }
    os << std::endl
       << "\tSt->_" << from
       << "[label=\"Start\"];"
       << std::endl;
    for (auto &_pair : trans)
    {
        int f = _pair.first;
        for (auto &_chr : _pair.second)
        {
            for (auto &_to : _chr.second)
            {
                os << "\t_" << f << "->_"
                   << _to
                   << "[label=\"";
                if (_chr.first == '\0')
                    os << "&epsilon;";
                else
                    os << _chr.first;
                os << "\"];" << std::endl;
            }
        }
    }
    os << "}" << std::endl;
    os.close();
}

node_set Graph::get_closure(node_set &s)
{
    node_set res = s;
    std::queue<int> task;
    for (auto &i : s)
    {
        task.push(i);
    }

    while (!task.empty())
    {
        int i = task.front();

        for (auto &j : trans[i]['\0'])
        {
            if (!res.count(j))
            {
                res.insert(j);
                task.push(j);
            }
        }
        task.pop();
    }
    return res;
}

Graph NFAGraph::nfa2dfa()
{
    Graph dfa;

    std::queue<node_set> sh;
    std::map<node_set, int> ss;

    node_set start_s({0});
    start_s = get_closure(start_s);
    sh.push(start_s);
    ss.insert(std::pair<node_set, int>(start_s, 0));
    dfa.add();

    while (sh.size())
    {
        node_set &f_set = sh.front();       // get the set
        std::map<char, node_set> dfs_trans; // create a empty trans
        int set_id = ss[f_set];

        for (auto &_node_id : f_set)
        { // for each node
            for (auto &_pair : trans[_node_id])
            { // for each char
                char c = _pair.first;
                if (c == '\0')
                    continue;
                for (auto &_to_id : _pair.second)
                { // for each to point
                    dfs_trans[c].insert(_to_id);
                }
            }
        }

        for (auto &_pair : dfs_trans)
        { // for each char
            char c = _pair.first;
            if (c == '\0')
                continue;
            node_set to_cset = get_closure(_pair.second);
            if (ss.count(to_cset))
            {
                dfa.add_edge(set_id, ss[to_cset], c);
            }
            else
            {
                int to_id = dfa.add();
                ss[to_cset] = to_id;
                sh.push(to_cset);
                dfa.add_edge(set_id, to_id, c);
            }
        }

        sh.pop();
    }

    for (auto &_set : ss)
    { // for each set
        for (auto &_n : _set.first)
        { // for each node of set
            if (tos.count(_n))
            {
                dfa.tos.insert(_set.second);
                break;
            }
        }
    }

    return dfa;
}

bool DFAGraph::check()
{
    for (auto &_pair : trans)
    {
        if (_pair.second['\0'].size() > 0)
            return false;
        for (auto &_char : _pair.second)
            if (_char.second.size() > 1)
                return false;
    }
    return true;
}

bool DFAGraph::match(std::string str)
{
    assert(check());
    int pos = 0, id = from;
    char c;
    for (; pos < str.length(); ++pos)
    {
        c = str[pos];
        if (!trans[id].count(c))
            return false;
        id = trans[id][c][0];
    }
    if (tos.find(id) == tos.end())
        return false;
    return true;
}

Graph::status Graph::operator[](std::string lbl)
{
    if (lbl == start)
        return status({*this, -1});
    int id = add_or_get(lbl);
    return status({*this, id});
}

Graph::status::hdl Graph::status::operator[](char c)
{
    return hdl({*this, c});
}

std::string Graph::status::operator=(std::string lbl)
{
    if (lbl == accepting)
    {
        assert(id >= 0);
        parent.tos.insert(id);
        return lbl;
    }
    if (id == -1)
    {
        id = parent.add_or_get(lbl);
        parent.from = id;
        return lbl;
    }
    parent.labels[id] = lbl;
    return lbl;
}

std::string Graph::status::hdl::operator=(std::string t_lbl)
{
    Graph &g = s.parent;
    int t_id = g.add_or_get(t_lbl);
    g.add_edge(s.id, t_id, c);
    return t_lbl;
}

} // namespace automata

#ifdef GRAPH_MAIN
using namespace automata;
using namespace std;

int main()
{
    string s = "ab*(a*|(ab)*)";
    RNDRegex r(s);
    Graph g(RNDRegex(r).reg2nfa());
    NFAGraph(s).print("nfa_str.dot");
    NFAGraph(r).print("nfa_reg.dot");
    NFAGraph(g).print("nfa_gra.dot");
    DFAGraph(s).print("dfa_str.dot");
    DFAGraph(r).print("dfa_reg.dot");
    DFAGraph(g).print("dfa_gra.dot");
    DFAGraph(g, automata::copy).print("dfa_cpy.dot");
}
#endif