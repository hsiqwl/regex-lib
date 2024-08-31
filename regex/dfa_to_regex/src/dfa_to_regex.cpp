#include "dfa_to_regex.h"

std::string dfa_to_regex::get_labels_for_simple_transition(size_t from, size_t to, const dfa &automaton) {
    std::string labels;
    auto &from_state = automaton.states[from];
    auto begin = from_state->get_valid_transitions().begin();
    auto end = from_state->get_valid_transitions().end();
    for (auto iter = begin; iter != end; ++iter) {
        if (iter->lock() == automaton.states[to]) {
            size_t char_code = std::distance(begin, iter);
            char symbol = static_cast<char>(char_code);
            std::string label(1, symbol);
            if(symbol == '.' || symbol == '*' || symbol == '+' || symbol == '?' || symbol == '{' || symbol == '[' || symbol == '}' || symbol == ']' || symbol == '|' || symbol == '%'){
                label.insert(label.begin(), '%');
            }
            labels = alternate(labels, label);
        }
    }
    if (from == to) {
        labels = alternate(labels, "$");
    }
    return labels;
}


std::string dfa_to_regex::convert_dfa_to_regex(const dfa &automaton) {
    paths.clear();
    new_paths.clear();
    for (size_t i = 0; i < automaton.states.size() - 1; ++i) {
        for (size_t j = 0; j < automaton.states.size() - 1; ++j) {
            paths[{i + 1, j + 1}] = get_labels_for_simple_transition(i, j, automaton);
        }
    }
    for (size_t k = 1; k <= automaton.states.size() - 1; ++k) {
        for (size_t i = 0; i < automaton.states.size() - 1; ++i) {
            for (size_t j = 0; j < automaton.states.size() - 1; ++j) {
                find_paths(i + 1, j + 1, k);
            }
        }
        paths = std::move(new_paths);
    }
    size_t initial = 0;
    for(; initial < automaton.states.size() - 1; ++initial) {
        if (automaton.states[initial] == automaton.initial_state) {
            ++initial;
            break;
        }
    }
    std::vector<size_t> accepting_states;
    for (auto iter = automaton.states.begin(); iter != automaton.states.end(); ++iter) {
        auto &s = *iter;
        if (s->is_accepting()) {
            accepting_states.emplace_back(std::distance(automaton.states.begin(), iter) + 1);
        }
    }
    return find_paths_to_accepting_states(accepting_states, initial);
}

/// Rijk = Rij(k-1) | Rik(k-1).(Rkk(k-1))*.Rkj(k-1)
void dfa_to_regex::find_paths(size_t from, size_t to, size_t path_number) {
    std::string &Rij_prev = paths[{from, to}];
    std::string &Rik_prev = paths[{from, path_number}];
    std::string &Rkk_prev = paths[{path_number, path_number}];
    std::string &Rkj_prev = paths[{path_number, to}];
    std::string new_path = alternate(Rij_prev, concatenate(concatenate(Rik_prev, repeat(Rkk_prev)), Rkj_prev));
    new_paths[{from, to}] = new_path;
}

std::string dfa_to_regex::find_paths_to_accepting_states(const std::vector<size_t> &accepting,
                                                         size_t initial) {
    std::string result_path;
    for (auto accept_state: accepting) {
        result_path = alternate(result_path, paths[{initial, accept_state}]);
    }
    return result_path;
}

bool dfa_to_regex::is_epsilon_string(const std::string &string) {
    return string == "$" || string == "($)";
}

std::string dfa_to_regex::concatenate(const std::string &first, const std::string &second) {
    if (first.empty() || second.empty())
        return {};
    if (is_epsilon_string(first))
        return second;
    if (is_epsilon_string(second))
        return first;
    return '(' + first + ")(" + second + ')';
}

std::string dfa_to_regex::repeat(const std::string &first) {
    if (first.empty()) {
        return {};
    }
    if(is_epsilon_string(first))
        return first;
    return append_operator(first, '*');
}

std::string dfa_to_regex::alternate(const std::string &first, const std::string &second) {
    if (first.empty() && second.empty())
        return {};
    if (first.empty())
        return second;
    if (second.empty())
        return first;
    if (is_epsilon_string(first) && is_epsilon_string(second))
        return first;
    if (is_epsilon_string(first))
        return append_operator(second, '?');
    if (is_epsilon_string(second))
        return append_operator(first, '?');
    if (first == second)
        return first;
    std::string result;
    result = '(' + first + ")|(" + second + ')';
    return result;
}

std::string dfa_to_regex::append_operator(const std::string &string, char operator_symbol) {
    if (string.size() == 1) {
        return string + operator_symbol;
    } else {
        if (string.back() == '?' && operator_symbol == '*') {
            return std::string(string.begin(), string.end() - 1) + '*';
        }
        return '(' + string + ')' + operator_symbol;
    }
}