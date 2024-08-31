#ifndef REGEX_DFA_MINIMIZER_H
#define REGEX_DFA_MINIMIZER_H
#include "dfa.h"
#include <unordered_map>
#include <vector>
#include <array>


class dfa_minimizer{
public:
    dfa_minimizer() = default;

    void minimize(dfa& automaton);
private:
    void initialize_correspondence_table(const std::vector<std::shared_ptr<state>>& states);

    void initialize_transition_tables(std::vector<std::pair<size_t, std::array<size_t, 256>>>& accepting_table,
                                             std::vector<std::pair<size_t, std::array<size_t, 256>>>& non_accepting_table,
                                             const dfa& automaton);

    std::unordered_map<std::shared_ptr<state>, size_t> state_to_number;

    std::unordered_map<size_t, size_t> equivalence_table;

    void restore_transitions(dfa& automaton);

    void minimize_table(std::vector<std::pair<size_t, std::array<size_t, 256>>>& table, const dfa& automaton);
};

#endif //REGEX_DFA_MINIMIZER_H
