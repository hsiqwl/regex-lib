#include "operator_info.h"

operator_info::operator_info(operator_info::operator_type oper_type) {
    op_type = oper_type;
    switch (op_type) {
        case operator_type::repetition:{
            precedence = 3;
            break;
        }
        case operator_type::concatenation:{
            precedence = 2;
            break;
        }
        case operator_type::alternation:{
            precedence = 1;
            break;
        }
    }
}

operator_info::operator_type operator_info::get_op_type() const noexcept {
    return op_type;
}

size_t operator_info::get_max_num_of_repetitions() const noexcept {
    return max_num_of_repetitions;
}

size_t operator_info::get_min_num_of_repetitions() const noexcept {
    return min_num_of_repetitions;
}

void operator_info::set_min_num_of_repetitions(size_t min_num) {
    if(op_type == operator_type::repetition)
        min_num_of_repetitions = min_num;
    else
        throw std::logic_error("Operator type must be repetition to set this attribute");
}

void operator_info::set_max_num_of_repetitions(size_t max_num) {
    if(op_type == operator_type::repetition)
        max_num_of_repetitions = max_num;
    else
        throw std::logic_error("Operator type must be repetition to set this attribute");
}

short operator_info::get_precedence() const noexcept {
    return precedence;
}

size_t operator_info::get_max_possible_num_of_repetitions() noexcept {
    return MAX_POSSIBLE_NUM_OF_REPETITIONS;
}