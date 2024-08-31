#include "dfa_minimizer.h"

void dfa_minimizer::minimize(dfa &automaton) {
    initialize_correspondence_table(automaton.states);
    std::vector<std::pair<size_t, std::array<size_t, 256>>> accepting_table;
    std::vector<std::pair<size_t, std::array<size_t, 256>>> non_accepting_table;
    initialize_transition_tables(accepting_table, non_accepting_table, automaton);
    minimize_table(accepting_table, automaton);
    minimize_table(non_accepting_table, automaton);
    restore_transitions(automaton);
}

void dfa_minimizer::initialize_correspondence_table(const std::vector<std::shared_ptr<state>> &states) {
    state_to_number.clear();
    for(auto iter = states.begin(); iter != states.end(); ++iter){
        state_to_number[*iter] = std::distance(states.begin(), iter);
    }
}

void dfa_minimizer::initialize_transition_tables(std::vector<std::pair<size_t, std::array<size_t, 256>>> &accepting_table,
                                                 std::vector<std::pair<size_t, std::array<size_t, 256>>> &non_accepting_table,
                                                 const dfa &automaton) {
    for (auto &state: automaton.states) {
        std::array<size_t, 256> transitions{};
        for (size_t i = 0; i < 256; ++i) {
            transitions[i] = state_to_number[state->get_following_state(i)];
        }
        if (state->is_accepting())
            accepting_table.emplace_back(state_to_number[state], transitions);
        else
            non_accepting_table.emplace_back(state_to_number[state], transitions);
    }
}

void dfa_minimizer::minimize_table(std::vector<std::pair<size_t, std::array<size_t, 256>>> &table, const dfa& automaton) {
    for (size_t i = 0; i < table.size(); ++i) {
        equivalence_table.insert({table[i].first, table[i].first});
        for (size_t j = 0; j < table.size(); ++j) {
            if (i != j && table[i].second == table[j].second &&
                table[j].first != state_to_number[automaton.initial_state]) {
                equivalence_table.insert({table[j].first, table[i].first});
                table.erase(table.begin() + j);
                --j;
            }
        }
    }
}

void dfa_minimizer::restore_transitions(dfa &automaton) {
    std::vector<std::shared_ptr<state>> new_states;
    for (int i = 0; i < automaton.states.size(); ++i) {
        if(equivalence_table[i] == i) {
            std::shared_ptr<state> state = automaton.states[i];
            for (int j = 0; j < 256; ++j) {
                size_t to_state = state_to_number[automaton.states[i]->get_following_state(j)];
                size_t new_to_state = equivalence_table[to_state];
                if (to_state != new_to_state)
                    state->add_transition(j, automaton.states[new_to_state]);
            }
            new_states.emplace_back(std::move(state));
        }
    }
    automaton.states = std::move(new_states);
}