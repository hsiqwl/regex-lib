#ifndef REGEX_OPERATOR_INFO_H
#define REGEX_OPERATOR_INFO_H
#include <cstddef>
#include <stdexcept>

class operator_info{
public:
    enum class operator_type{
        concatenation,
        alternation,
        repetition
    };

    operator_info(operator_type oper_type);

    void set_min_num_of_repetitions(size_t min_num = 0);

    void set_max_num_of_repetitions(size_t max_num = MAX_POSSIBLE_NUM_OF_REPETITIONS);

    size_t get_min_num_of_repetitions() const noexcept;

    size_t get_max_num_of_repetitions() const noexcept;

    operator_type get_op_type() const noexcept;

    short get_precedence() const noexcept;

    static size_t get_max_possible_num_of_repetitions() noexcept;

private:
    static const size_t MAX_POSSIBLE_NUM_OF_REPETITIONS = -1;

    operator_type op_type;

    size_t min_num_of_repetitions;

    size_t max_num_of_repetitions;

    short precedence;
};

#endif //REGEX_OPERATOR_INFO_H
