#ifndef REGEX_DFA_TO_REGEX_H
#define REGEX_DFA_TO_REGEX_H
#include <string>
#include "dfa.h"
#include <unordered_map>
#include <boost/functional/hash/hash.hpp>

class dfa_to_regex{
public:
    static std::string convert_dfa_to_regex(const dfa& automaton);

private:
    inline static std::unordered_map<std::pair<size_t,size_t>, std::string, boost::hash<std::pair<size_t, size_t>>> paths{};

    inline static std::unordered_map<std::pair<size_t,size_t>, std::string, boost::hash<std::pair<size_t, size_t>>> new_paths{};

    static bool is_epsilon_string(const std::string& string);

    static std::string concatenate(const std::string& first, const std::string& second);

    static std::string repeat(const std::string& first);

    static std::string alternate(const std::string& first, const std::string& second);

    static std::string append_operator(const std::string& string, char operator_symbol);

    static std::string get_labels_for_simple_transition(size_t from, size_t to, const dfa& automaton);

    static void find_paths(size_t from, size_t to, size_t path_number);

    static std::string find_paths_to_accepting_states(const std::vector<size_t>& accepting, size_t initial);
};

#endif //REGEX_DFA_TO_REGEX_H
