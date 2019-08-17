#include "regex.h"
#include "graph.h"

using namespace automata;
using namespace std;

int main(int argc, char **argv)
{
    string s = "ab*(a*|(ab)*)";
    NFAGraph g(s);
    cout << g;
    g.export_dot("nfa.dot");
    DFAGraph d(s);
    d.export_dot("dfa.dot");
    cout << d.match("") << endl;
    cout << d.match("abbbbaaaaa") << endl;
    cout << d.match("abbabb") << endl;
    cout << d.match("aa") << endl;
    cout << d.match("aabababab") << endl;
    return 0;
}