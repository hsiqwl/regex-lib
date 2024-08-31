#include "dfa.h"

bool dfa::is_in_accepting_state() const noexcept {
    return curr_state->is_accepting();
}

bool dfa::is_in_error_state() const noexcept {
    return curr_state->is_error_state();
}

void dfa::reset() {
    curr_state = initial_state;
}

dfa::dfa(const dfa &other) {
    std::unordered_map<std::shared_ptr<state>, std::shared_ptr<state>> correspondence_table;
    std::for_each(other.states.begin(), other.states.end(),
                  [&correspondence_table](const std::shared_ptr<state> &copied_state) {
                      correspondence_table[copied_state] = std::make_shared<state>(copied_state->is_accepting());
                      if (copied_state->is_error_state()) {
                          correspondence_table[copied_state]->declare_as_error_state();
                      }
                  });
    states.resize(other.states.size());
    std::transform(other.states.begin(), other.states.end(), states.begin(),
                   [&correspondence_table](const std::shared_ptr<state> &other_state) {
                       for (unsigned short i = 0; i < 256; ++i) {
                           auto to_state = other_state->get_valid_transitions()[i].lock();
                           correspondence_table[other_state]->add_transition(i, correspondence_table[to_state]);
                       }
                       return correspondence_table[other_state];
                   });
    for (auto &st: other.accepting_states) {
        accepting_states.push_back(correspondence_table[st]);
    }
    initial_state = correspondence_table[other.initial_state];
    curr_state = correspondence_table[other.curr_state];
}

void dfa::consume_input(char c) noexcept {
    curr_state = curr_state->get_following_state(c);
}