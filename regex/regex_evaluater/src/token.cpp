#include "token.h"

token::token(token::token_type type): type(type) {}

token::token_type token::get_type() const noexcept {
    return type;
}

void token::set_operator_info(const operator_info &info) {
    if(type == token_type::op)
        token_info.emplace<operator_info>(info);
    else
        throw std::logic_error("Token type must be token::token_type::op");
}

void token::set_terminal_info(const terminal_info &info) {
    if(type == token_type::terminal)
        token_info.emplace<terminal_info>(info);
    else
        throw std::logic_error("Token type must be token::token_type::terminal");
}

const operator_info &token::get_operator_info() const{
    return std::get<operator_info>(token_info);
}

const terminal_info &token::get_terminal_info() const{
    return std::get<terminal_info>(token_info);
}

const group_info &token::get_group_info() const {
    return tracked_groups;
}

void token::add_group_to_tracked_groups(size_t group_number) {
    tracked_groups.add_group(group_number);
}

void token::set_group_repetitive() {
    tracked_groups.set_repetitive();
}