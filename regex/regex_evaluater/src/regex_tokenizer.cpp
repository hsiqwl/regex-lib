#include "regex_tokenizer.h"
#include <iostream>
token regex_tokenizer::get_repetition_token_ptr(std::string::iterator& iter) {
    token repetition_token(token::token_type::op);
    operator_info info(operator_info::operator_type::repetition);
    switch (*iter) {
        case '+': {
            info.set_min_num_of_repetitions(1);
            info.set_max_num_of_repetitions();
            break;
        }
        case '*': {
            info.set_min_num_of_repetitions();
            info.set_max_num_of_repetitions();
            break;
        }
        case '?': {
            info.set_min_num_of_repetitions();
            info.set_max_num_of_repetitions(1);
            break;
        }
        case '{': {
            size_t min_rep, max_rep;
            parse_repetition_operator(++iter, min_rep, max_rep);
            info.set_min_num_of_repetitions(min_rep);
            info.set_max_num_of_repetitions(max_rep);
            break;
        }
    }
    repetition_token.set_operator_info(info);
    return repetition_token;
}

void regex_tokenizer::parse_repetition_operator(std::string::iterator& iter, size_t& min_rep, size_t& max_rep) {
    min_rep = 0;
    max_rep = 0;
    bool first_value = true;
    for (; *iter != '}'; ++iter) {
        if (*iter == ' ')
            continue;
        if (*iter >= '0' && *iter <= '9' && first_value) {
            min_rep *= 10;
            min_rep += static_cast<int>(*iter) - static_cast<int>('0');
        } else if (*iter >= '0' && *iter <= '9' && !first_value) {
            max_rep *= 10;
            max_rep += static_cast<int>(*iter) - static_cast<int>('0');
        } else if (*iter == ',') {
            first_value = false;
        } else {
            throw std::invalid_argument("Invalid repetition operator");
        }
    }
    if (min_rep > max_rep) {
        throw std::invalid_argument("Invalid repetition operator");
    }
}

void regex_tokenizer::parse_character_class_terminal(std::string::iterator& iter, std::string &singles, char &range_min,
                                                     char &range_max) {
    bool single_range = true;
    singles.push_back(*iter);
    ++iter;
    for (; *iter != ']'; ++iter) {
        if (*iter == '-' && single_range) {
            if (*(iter + 1) == ']') {
                singles.push_back(*iter);
                single_range = true;
            } else {
                range_min = singles.back();
                singles.pop_back();
                range_max = *(iter + 1);
                single_range = false;
                ++iter;
                continue;
            }
        } else if (*iter == '-' && !single_range) {
            if ((*iter + 1) == ']') {
                singles.push_back(*iter);
            } else {
                throw std::invalid_argument("Invalid character class");
            }
        } else {
            singles.push_back(*iter);
        }
    }
}

void regex_tokenizer::turn_into_token_sequence(std::string expression) {
    bool after_escape_character = false;
    for(auto it = expression.begin(); it != expression.end(); ++it){
        char c = *it;
        if(c == '%' && !after_escape_character){
            after_escape_character = true;
        }else if((c == '*' || c == '+' || c == '?' || c == '{') && !after_escape_character){
            if(c == '{' && expression.find('}', std::distance(expression.begin(), it)) == std::string_view::npos){
                throw std::invalid_argument("Invalid repetition operator");
            }
            token_sequence.emplace_back(get_repetition_token_ptr(it));
        }else if(c == '.' && !after_escape_character){
            token concat_token(token::token_type::op);
            operator_info op_info(operator_info::operator_type::concatenation);
            concat_token.set_operator_info(op_info);
            token_sequence.emplace_back(std::move(concat_token));
        }else if(c == '|' && !after_escape_character){
            token alternation_token(token::token_type::op);
            operator_info op_info(operator_info::operator_type::alternation);
            alternation_token.set_operator_info(op_info);
            token_sequence.emplace_back(std::move(alternation_token));
        }
        else if(c == '(' && !after_escape_character){
            token_sequence.emplace_back(token::token_type::left_parenthesis);
        }
        else if(c == ')' && !after_escape_character){
            token_sequence.emplace_back(token::token_type::right_parenthesis);
        }else if(c == '[' && !after_escape_character){
            if(expression.find(']', std::distance(expression.begin(), it))  == std::string_view::npos){
                throw std::invalid_argument("Invalid character class");
            }else{
                std::string singles;
                char range_min;
                char range_max;
                parse_character_class_terminal(++it, singles, range_min, range_max);
                token terminal_token(token::token_type::terminal);
                terminal_info term_info(range_min, range_max, singles);
                terminal_token.set_terminal_info(term_info);
                token_sequence.emplace_back(std::move(terminal_token));
            }
        }
        else{
            token terminal_token(token::token_type::terminal);
            terminal_info term_info(c);
            terminal_token.set_terminal_info(term_info);
            token_sequence.emplace_back(std::move(terminal_token));
            after_escape_character = false;
        }
    }
    add_concat_tokens();
}

void regex_tokenizer::add_concat_tokens() {
    for (auto iter = token_sequence.begin() + 1; iter < token_sequence.end(); ++iter) {
        token& prev_token = *(iter - 1);
        token& curr_token = *iter;
        bool first_predicate = curr_token.get_type() == token::token_type::left_parenthesis ||
                               curr_token.get_type() == token::token_type::terminal;
        if(!first_predicate)
            continue;
        bool second_predicate = prev_token.get_type() == token::token_type::op;
        if(second_predicate) {
            second_predicate = prev_token.get_operator_info().get_op_type() == operator_info::operator_type::repetition;
        }
        bool third_predicate = prev_token.get_type() == token::token_type::right_parenthesis ||
                               prev_token.get_type() == token::token_type::terminal;
        if (second_predicate || third_predicate) {
            token concat_token(token::token_type::op);
            operator_info op_info(operator_info::operator_type::concatenation);
            concat_token.set_operator_info(op_info);
            iter = token_sequence.emplace(iter, std::move(concat_token));
            ++iter;
        }
    }
}

void regex_tokenizer::assert_expression() {
    mismatched_parenthesis = 0;
    assert_first_token(token_sequence.front());
    assert_last_token(token_sequence.back());
    if(token_sequence.front().get_type() == token::token_type::left_parenthesis) {
        mismatched_parenthesis++;
    }
    if(token_sequence.back().get_type() == token::token_type::right_parenthesis) {
        mismatched_parenthesis--;
    }
    for (auto iter = token_sequence.begin() + 1; iter < token_sequence.end() - 1; ++iter) {
        if (iter->get_type() == token::token_type::op) {
            switch (iter->get_operator_info().get_op_type()) {
                case operator_info::operator_type::concatenation:{
                    assert_concatenation_operation(*(iter - 1), *(iter + 1));
                    break;
                }
                case operator_info::operator_type::alternation:{
                    assert_alternation_operation(*(iter - 1), *(iter + 1));
                    break;
                }
                case operator_info::operator_type::repetition:{
                    assert_star_operation(*(iter - 1));
                    break;
                }
            }
        }
        if (iter->get_type() == token::token_type::left_parenthesis) {
            mismatched_parenthesis++;
            assert_first_token(*(iter + 1));
        }
        if (iter->get_type() == token::token_type::right_parenthesis) {
            mismatched_parenthesis--;
            assert_last_token(*(iter - 1));
        }
    }
    if (mismatched_parenthesis != 0)
        throw std::invalid_argument("Mismatched parenthesis");
}


void regex_tokenizer::assert_first_token(const token &first_token) {
    if (first_token.get_type() == token::token_type::op || first_token.get_type() == token::token_type::right_parenthesis)
        throw std::exception();
}

void regex_tokenizer::assert_last_token(const token &last_token) {
    if (last_token.get_type() == token::token_type::op) {
        auto op_type = last_token.get_operator_info().get_op_type();
        if(op_type == operator_info::operator_type::alternation || op_type == operator_info::operator_type::concatenation){
            throw std::invalid_argument("Last symbol can't be alternation or concatenation operation");
        }
    }
    if (last_token.get_type() == token::token_type::left_parenthesis)
        throw std::invalid_argument("Last symbol can't be left parenthesis");
}

void regex_tokenizer::assert_star_operation(const token &left_hand_token) {
    if(left_hand_token.get_type() != token::token_type::terminal && left_hand_token.get_type() != token::token_type::right_parenthesis)
        throw std::invalid_argument("Invalid repetition operator operand");
}

void regex_tokenizer::assert_alternation_operation(const token &left_hand_token, const token &right_hand_token) {
    assert_last_token(left_hand_token);
    assert_first_token(right_hand_token);
}

void regex_tokenizer::assert_concatenation_operation(const token &left_hand_token, const token &right_hand_token) {
    bool first_predicate = right_hand_token.get_type() == token::token_type::left_parenthesis ||
                           right_hand_token.get_type() == token::token_type::terminal;
    bool second_predicate = left_hand_token.get_type() == token::token_type::op;
    if(second_predicate){
        second_predicate = left_hand_token.get_operator_info().get_op_type() == operator_info::operator_type::repetition;
    }
    bool third_predicate = left_hand_token.get_type() == token::token_type::right_parenthesis ||
                           left_hand_token.get_type() == token::token_type::terminal;
    if (!(first_predicate && (second_predicate || third_predicate))) {
        throw std::invalid_argument("Invalid concatenation operation operand");
    }
}

regex_tokenizer::regex_tokenizer(std::string expression) {
    mismatched_parenthesis = 0;
    turn_into_token_sequence(std::move(expression));
    assert_expression();
    infix_to_postfix({token_sequence.begin(), token_sequence.end()});
}

const std::vector<token> &regex_tokenizer::get_token_sequence() const noexcept {
    return token_sequence;
}

void regex_tokenizer::infix_to_postfix(std::pair<token_iterator, token_iterator>&& iterators) {
    std::stack<token> operator_stack;
    std::vector<token> postfix_token_sequence;
    for(auto& token_ptr = iterators.first; token_ptr != iterators.second; ++token_ptr) {
        switch (token_ptr->get_type()) {
            case token::token_type::terminal:
                handle_terminal(postfix_token_sequence, *token_ptr);
                break;
            case token::token_type::left_parenthesis:
                handle_left_parenthesis(operator_stack, *token_ptr);
                break;
            case token::token_type::op:
                handle_operator(operator_stack, postfix_token_sequence, *token_ptr);
                break;
            case token::token_type::right_parenthesis:
                handle_right_parenthesis(operator_stack, postfix_token_sequence);
                break;
        }
    }
    while (!operator_stack.empty()) {
        postfix_token_sequence.emplace_back(std::move(operator_stack.top()));
        operator_stack.pop();
    }
    token_sequence = postfix_token_sequence;
}

void regex_tokenizer::handle_terminal(std::vector<token>& postfix_token_sequence, const token& terminal_token) {
    postfix_token_sequence.emplace_back(terminal_token);
}

void regex_tokenizer::handle_left_parenthesis(std::stack<token> &operator_stack, const token& parenthesis_token) {
    group_number_stack.push(group_counter++);
    operator_stack.push(parenthesis_token);
}

void regex_tokenizer::handle_operator(std::stack<token> &operator_stack, std::vector<token> &postfix_token_sequence,
                                  const token& op_token) {
    while (!operator_stack.empty() && operator_stack.top().get_type() != token::token_type::left_parenthesis) {
        unsigned short top_op_precedence = operator_stack.top().get_operator_info().get_precedence();
        unsigned short curr_op_precedence = op_token.get_operator_info().get_precedence();
        if (top_op_precedence >= curr_op_precedence) {
            postfix_token_sequence.emplace_back(std::move(operator_stack.top()));
            operator_stack.pop();
        } else {
            break;
        }
    }
    operator_stack.push(op_token);
}

void regex_tokenizer::handle_right_parenthesis(std::stack<token> &operator_stack, std::vector<token> &postfix_token_sequence) {
    while (operator_stack.top().get_type() != token::token_type::left_parenthesis) {
        postfix_token_sequence.emplace_back(std::move(operator_stack.top()));
        operator_stack.pop();
    }
    size_t sequence_size = postfix_token_sequence.size();
    auto &last_token = postfix_token_sequence[sequence_size - 1];
    last_token.add_group_to_tracked_groups(group_number_stack.top());
    bool is_repetitive = last_token.get_type() == token::token_type::op
                         && last_token.get_operator_info().get_op_type() == operator_info::operator_type::repetition
                         && last_token.get_operator_info().get_max_num_of_repetitions() ==
                            operator_info::get_max_possible_num_of_repetitions();
    group_number_stack.pop();
    if (is_repetitive) {
        last_token.set_group_repetitive();
    }
    operator_stack.pop();
}


