#ifndef REGEX_REGEX_EVALUATER_H
#define REGEX_REGEX_EVALUATER_H

#include <string_view>
#include <vector>
#include <stack>
#include <memory>
#include <algorithm>
#include "token.h"

class regex_tokenizer{
private:
    std::stack<size_t> group_number_stack;

    std::vector<token> token_sequence;

    size_t mismatched_parenthesis = 0;

    size_t group_counter = 1;
public:
    typedef std::vector<token>::const_iterator token_iterator;

    regex_tokenizer(std::string expression);

    [[nodiscard]] const std::vector<token>& get_token_sequence() const noexcept;

private:
    token get_repetition_token_ptr(std::string::iterator& iter);

    void parse_repetition_operator(std::string::iterator& iter, size_t& min_rep, size_t& max_rep);

    void parse_character_class_terminal(std::string::iterator& iter, std::string& singles, char& range_min, char& range_max);

    void turn_into_token_sequence(std::string expression);

    void add_concat_tokens();

    void assert_first_token(const token& token);

    void assert_last_token(const token& token);

    void assert_star_operation(const token& left_hand_token);

    void assert_alternation_operation(const token& left_hand_token, const token& right_hand_token);

    void assert_concatenation_operation(const token& left_hand_token, const token& right_hand_token);

    void assert_expression();

    void handle_operator(std::stack<token>& operator_stack, std::vector<token>& postfix_token_sequence, const token& op_token);

    void handle_left_parenthesis(std::stack<token>& operator_stack, const token& parenthesis_token);

    void handle_right_parenthesis(std::stack<token>& operator_stack, std::vector<token>& postfix_token_sequence);

    void handle_terminal(std::vector<token>& postfix_token_sequence,const token& terminal_token);

    void infix_to_postfix(std::pair<token_iterator, token_iterator>&& iterators);
};

#endif