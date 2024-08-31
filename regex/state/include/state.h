#ifndef REGEX_STATE_H
#define REGEX_STATE_H
#include <array>
#include <memory>

class state{
public:
    state() = default;

    state(bool acceptance);

    [[nodiscard]] bool is_accepting() const noexcept;

    void add_transition(char c, const std::shared_ptr<state>& to_state);

    std::shared_ptr<state> get_following_state(char c);

    void set_acceptance(bool acceptance);

    [[nodiscard]] const std::array<std::weak_ptr<state>, 256>& get_valid_transitions() const; //iterators

    void declare_as_error_state();

    bool is_error_state() const noexcept;

private:
    std::array<std::weak_ptr<state>, 256> transitions;

    bool accepting = false;

    bool error_state = false;
};

#endif //REGEX_STATE_H
