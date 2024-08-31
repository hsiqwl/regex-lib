#ifndef REGEX_TOKEN_H
#define REGEX_TOKEN_H
#include <vector>
#include <variant>
#include <algorithm>
#include <memory>
#include "terminal_info.h"
#include "operator_info.h"
#include "group_info.h"

class token{
public:
    enum class token_type{
        op,
        left_parenthesis,
        right_parenthesis,
        terminal
    };

    explicit token(token_type type);

    [[nodiscard]] token_type get_type() const noexcept;

    void set_operator_info(const operator_info& info);

    void set_terminal_info(const terminal_info& info);

    void add_group_to_tracked_groups(size_t group_number);

    void set_group_repetitive();

    [[nodiscard]] const terminal_info& get_terminal_info() const;

    [[nodiscard]] const operator_info& get_operator_info() const;

    [[nodiscard]] const group_info& get_group_info() const;

private:
    token_type type;

    group_info tracked_groups;

    std::variant<terminal_info, operator_info> token_info;
};

#endif //REGEX_TOKEN_H
