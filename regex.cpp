#include "regex.h"

namespace automata
{
void RNDRegex::reg_concat()
{
    subgraph right = st_opr.top();
    st_opr.pop();
    subgraph left = st_opr.top();
    st_opr.pop();

    subgraph res(right, left);
    res.add_edge(left.to, right.from);
    res.from = left.from;
    res.to = right.to;

    st_opr.push(res);
}

void RNDRegex::reg_union()
{
    subgraph right = st_opr.top();
    st_opr.pop();
    subgraph left = st_opr.top();
    st_opr.pop();

    subgraph res(right, left);
    int f = res.from = new_status();
    int t = res.to = new_status();

    res.add_edge(f, right.from);
    res.add_edge(f, left.from);
    res.add_edge(right.to, t);
    res.add_edge(left.to, t);

    st_opr.push(res);
}

void RNDRegex::reg_star()
{
    subgraph res = st_opr.top();
    st_opr.pop();

    int f = new_status();
    int t = new_status();

    res.add_edge(res.to, res.from);
    res.add_edge(f, res.from);
    res.add_edge(res.to, t);
    res.add_edge(f, t);
    res.from = f;
    res.to = t;
    st_opr.push(res);
}

void RNDRegex::push_opr(char c)
{
    int f = new_status();
    int t = new_status();

    st_opr.push(subgraph(f, t, c));
}

bool is_opt(char c, std::string e = "")
{
    if (e.find(c) != std::string::npos)
        return false;
    return (c == '(' || c == ')' || c == CONCAT ||
            c == '|' || c == '*');
}

void RNDRegex::simple_regex()
{
    std::string res = exp;
    for (int i = 0; i < res.length(); ++i)
    {
        char n = res[i + 1];
        if (n != '\0' && !is_opt(res[i], "*)") && !is_opt(n, "("))
        {
            res.insert(++i, 1, CONCAT);
        }
    }
    exp = res;
}

subgraph RNDRegex::reg2nfa()
{
    std::map<char, int> priority = {
        {'(', 0},
        {'|', 1},
        {CONCAT, 2},
        {'*', 3},
    };

    simple_regex();
    for (int i = 0; i < exp.length(); ++i)
    {
        char c = exp[i];
        switch (c)
        {
        case '(':
            st_opt.push(c);
            break;
        case '|':
        case '*':
        case CONCAT:
            while (!st_opt.empty() && priority[c] < priority[st_opt.top()])
                reg_eval();
            st_opt.push(c);
            break;
        case ')':
            while (st_opt.top() != '(')
                reg_eval();
            st_opt.pop();
            break;
        default:
            push_opr(c);
            break;
        }
    }

    while (!st_opt.empty())
        reg_eval();

    return st_opr.top();
}

void RNDRegex::reg_eval()
{
    if (!st_opt.empty())
    {
        char op = st_opt.top();
        st_opt.pop();
        switch (op)
        {
        case '|':
            reg_union();
            break;

        case '*':
            reg_star();
            break;

        case CONCAT:
            reg_concat();
            break;

        default:
            std::cout << "Opt is not support." << std::endl;
            break;
        }
    }
}

} // namespace automata---

#ifdef REGEX_MAIN
using namespace automata;
int main()
{
    RNDRegex r("ab*(a*|(ab)*)");
    std::cout << r.reg2nfa();
    return 0;
}
#endif