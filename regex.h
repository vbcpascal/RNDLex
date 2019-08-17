#ifndef __Regex_H__
#define __Regex_H__

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <map>

#define CONCAT '.'

namespace automata
{

class subgraph
{
public:
    int from, to;
    std::map<int, std::map<char, std::vector<int>>> trans;

    void add_edge(int f, int t, char c = '\0')
    {
        trans[f][c].push_back(t);
    }
    subgraph() = default;
    subgraph(const subgraph &s) = default;
    subgraph(const subgraph &a, const subgraph &b)
    {
        for (auto &i : a.trans)
            trans.insert(i);
        for (auto &i : b.trans)
            trans.insert(i);
    }
    subgraph(int f, int t, char c = '\0') : from(f), to(t)
    {
        add_edge(f, t, c);
    };

    friend std::ostream &operator<<(std::ostream &os, const subgraph &sg);
};

class RNDRegex
{
private:
    int sum = 0;
    std::string exp = "";
    std::stack<char> st_opt;
    std::stack<subgraph> st_opr;
    int new_status() { return sum++; }
    void simple_regex();
    void reg_eval();
    void push_opr(char c);
    void reg_union();
    void reg_star();
    void reg_concat();

public:
    subgraph reg2nfa();

    RNDRegex() = default;
    RNDRegex(const RNDRegex &r) = default;
    RNDRegex(std::string s) : exp(s) {}
};

} // namespace automata

#endif