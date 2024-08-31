#ifndef REGEX_AST_BUILDER_H
#define REGEX_AST_BUILDER_H
#include "ast.h"
#include "regex_tokenizer.h"

class ast_builder{
public:
    static ast tokens_to_ast(const std::pair<regex_tokenizer::token_iterator, regex_tokenizer::token_iterator>& iterators);

    static ast get_reverse_ast(const ast& tree);
private:
    inline static char end_symbol = '#';

    static void operation_to_ast(const operator_info& info, std::stack<std::shared_ptr<ast>>& subtree);

    static void concat_to_ast(std::stack<std::shared_ptr<ast>>& subtree);

    static void alternative_to_ast(std::stack<std::shared_ptr<ast>>& subtree);

    static void star_to_ast(std::stack<std::shared_ptr<ast>>& subtree);

    static void terminal_to_ast(const terminal_info& info, std::stack<std::shared_ptr<ast>>& subtree);

    static void character_class_to_ast(const terminal_info& info, std::stack<std::shared_ptr<ast>>& subtree);

    static void single_character_to_ast(const terminal_info& info, std::stack<std::shared_ptr<ast>>& subtree);

    static void repetition_to_ast(const operator_info& info, std::stack<std::shared_ptr<ast>>& subtree);

    static void left_open_range_to_ast(size_t upper_bound, std::stack<std::shared_ptr<ast>>& subtree);

    static void closed_range_to_ast(size_t lower_bound, size_t upper_bound, std::stack<std::shared_ptr<ast>>& subtree);

    static void right_open_range_to_ast(size_t lower_bound, std::stack<std::shared_ptr<ast>>& subtree);
};

#endif //REGEX_AST_BUILDER_H
