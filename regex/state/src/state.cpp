#include "state.h"

state::state(bool acceptance) {
    accepting = acceptance;
}

void state::set_acceptance(bool acceptance) {
    accepting = acceptance;
}

void state::add_transition(char c, const std::shared_ptr<state> &to_state) {
    auto sym = static_cast<unsigned char>(c);
    auto index = static_cast<unsigned short>(sym);
    transitions[index] = to_state;
}

std::shared_ptr<state> state::get_following_state(char c) {
    auto sym = static_cast<unsigned char>(c);
    auto index = static_cast<unsigned short>(sym);
    return transitions[index].lock();
}

bool state::is_accepting() const noexcept {
    return accepting;
}

const std::array<std::weak_ptr<state>, 256>& state::get_valid_transitions() const {
    return transitions;
}

void state::declare_as_error_state() {
    error_state = true;
}

bool state::is_error_state() const noexcept {
    return error_state;
}