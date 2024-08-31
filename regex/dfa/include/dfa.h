#ifndef REGEX_DFA_H
#define REGEX_DFA_H
#include "state.h"
#include <functional>
#include <vector>
#include <unordered_map>
#include <algorithm>

class dfa_builder;

class dfa_to_regex;

class dfa_minimizer;

class dfa {
public:
    friend class dfa_builder;

    friend class dfa_to_regex;

    friend class dfa_minimizer;

    dfa() = default;

    dfa(const dfa& other);

    void consume_input(char c) noexcept;

    [[nodiscard]] bool is_in_error_state() const noexcept;

    [[nodiscard]] bool is_in_accepting_state() const noexcept;

    void reset();

private:
    std::vector<std::shared_ptr<state>> states;

    std::vector<std::shared_ptr<state>> accepting_states;

    std::shared_ptr<state> curr_state;

    std::shared_ptr<state> initial_state;
};

#endif //REGEX_DFA_H
