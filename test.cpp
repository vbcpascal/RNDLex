template <typename T>
struct identity
{
    T value;
    operator T() const { return value; }
};

template <template <typename> C>
class graph
{
    std::map<int, std::map<char, C<int>>> ssjf;
};

nfa->graph<std::vector>;
dfa->graph<identity>;