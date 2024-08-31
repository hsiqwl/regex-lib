#include "ast_builder.h"

ast ast_builder::tokens_to_ast(const std::pair<regex_tokenizer::token_iterator, regex_tokenizer::token_iterator>& iterators) {
    std::stack<std::shared_ptr<ast>> subtree;
    for (auto iter = iterators.first; iter < iterators.second; ++iter) {
        if (iter->get_type() == token::token_type::terminal)
            terminal_to_ast(iter->get_terminal_info(), subtree);
        else
            operation_to_ast(iter->get_operator_info(), subtree);
        subtree.top()->get_root().set_group_info(iter->get_group_info());
    }
    std::shared_ptr<ast> left = subtree.top();
    subtree.pop();
    std::shared_ptr<node> node_ptr = std::make_shared<node>(end_symbol);
    std::shared_ptr<ast> right = std::make_shared<ast>(node_ptr);
    subtree.emplace(std::make_shared<ast>(node::node_type::concat, *left, *right));
    group_info root_info(true);
    root_info.add_group(0);
    subtree.top()->get_root().set_group_info(root_info);
    return *subtree.top();
}

void ast_builder::operation_to_ast(const operator_info& info, std::stack<std::shared_ptr<ast>>& subtree) {
    switch (info.get_op_type()) {
        case operator_info::operator_type::concatenation: {
            concat_to_ast(subtree);
            break;
        }
        case operator_info::operator_type::alternation: {
            alternative_to_ast(subtree);
            break;
        }
        case operator_info::operator_type::repetition: {
            repetition_to_ast(info, subtree);
            break;
        }
    }
}

void ast_builder::concat_to_ast(std::stack<std::shared_ptr<ast>>& subtree){
    std::shared_ptr<ast> right = subtree.top();
    subtree.pop();
    std::shared_ptr<ast> left = subtree.top();
    subtree.pop();
    subtree.emplace(std::make_shared<ast>(node::node_type::concat, *left, *right));
}

void ast_builder::alternative_to_ast(std::stack<std::shared_ptr<ast>>& subtree){
    std::shared_ptr<ast> right = subtree.top();
    subtree.pop();
    std::shared_ptr<ast> left = subtree.top();
    subtree.pop();
    subtree.emplace(std::make_shared<ast>(node::node_type::alternation, *left, *right));
}

void ast_builder::star_to_ast(std::stack<std::shared_ptr<ast>>& subtree){
    std::shared_ptr<ast> sub_tree = subtree.top();
    subtree.pop();
    subtree.emplace(std::make_shared<ast>(node::node_type::star, *sub_tree));
}

void ast_builder::terminal_to_ast(const terminal_info& info, std::stack<std::shared_ptr<ast>>& subtree) {
    if (info.is_character_class()) {
        character_class_to_ast(info, subtree);
    } else {
        single_character_to_ast(info, subtree);
    }
}

void ast_builder::single_character_to_ast(const terminal_info& info, std::stack<std::shared_ptr<ast>>& subtree){
    std::shared_ptr<node> root = std::make_shared<node>(info.get_range_min());
    subtree.emplace(std::make_shared<ast>(root));
}

void ast_builder::character_class_to_ast(const terminal_info& info, std::stack<std::shared_ptr<ast>>& subtree) {
    std::shared_ptr<node> root = std::make_shared<node>(info.get_range_min());
    std::shared_ptr<ast> tree = std::make_shared<ast>(root);
    size_t start = (size_t)info.get_range_min() + 1;
    size_t end = (size_t)info.get_range_max();
    for (; start <= end; ++start) {
        root = std::make_shared<node>((char) start);
        std::shared_ptr<ast> tree_from_curr_root = std::make_shared<ast>(root);
        tree = std::make_shared<ast>(node::node_type::alternation, *tree, *tree_from_curr_root);
    }
    for (char c: info.get_singles()) {
        root = std::make_shared<node>(c);
        std::shared_ptr<ast> tree_from_curr_root = std::make_shared<ast>(root);
        tree = std::make_shared<ast>(node::node_type::alternation, *tree, *tree_from_curr_root);
    }
    subtree.push(tree);
}

void ast_builder::repetition_to_ast(const operator_info& info, std::stack<std::shared_ptr<ast>>& subtree) {
    size_t min = info.get_min_num_of_repetitions();
    size_t max = info.get_max_num_of_repetitions();
    if (max == operator_info::get_max_possible_num_of_repetitions() && min == 0) {
        star_to_ast(subtree);
    } else if (min == 0) {
        left_open_range_to_ast(max, subtree);
        return;
    } else if (max == operator_info::get_max_possible_num_of_repetitions()) {
        right_open_range_to_ast(min, subtree);
    } else {
        closed_range_to_ast(min, max, subtree);
    }
}

void ast_builder::closed_range_to_ast(size_t lower_bound, size_t upper_bound, std::stack<std::shared_ptr<ast>>& subtree) {
    std::shared_ptr<ast> copied_ast = std::make_shared<ast>(subtree.top()->get_deep_copy());
    std::shared_ptr<ast> accumulated = subtree.top();
    subtree.pop();
    for (size_t i = 0; i + 1 < lower_bound; ++i) {
        std::shared_ptr<ast> copy = std::make_shared<ast>(copied_ast->get_deep_copy());
        accumulated = std::make_shared<ast>(node::node_type::concat, *accumulated, *copy);
    }
    for (size_t i = lower_bound; i < upper_bound; ++i) {
        std::shared_ptr<ast> copy = std::make_shared<ast>(copied_ast->get_deep_copy());
        copy = std::make_shared<ast>(node::node_type::optional, *copy);
        accumulated = std::make_shared<ast>(node::node_type::concat, *accumulated, *copy);
    }
    subtree.emplace(accumulated);
}

void ast_builder::right_open_range_to_ast(size_t lower_bound, std::stack<std::shared_ptr<ast>>& subtree) {
    std::shared_ptr<ast> copy = std::make_shared<ast>(subtree.top()->get_deep_copy());
    closed_range_to_ast(lower_bound, lower_bound, subtree);
    copy = std::make_shared<ast>(node::node_type::star, *copy);
    std::shared_ptr<ast> top = subtree.top();
    subtree.pop();
    subtree.emplace(std::make_shared<ast>(node::node_type::concat, *top, *copy));
}

void ast_builder::left_open_range_to_ast(size_t upper_bound, std::stack<std::shared_ptr<ast>>& subtree){
    closed_range_to_ast(1, upper_bound, subtree);
    std::shared_ptr<ast> top = subtree.top();
    subtree.pop();
    subtree.emplace(std::make_shared<ast>(node::node_type::optional, *top));
}


ast ast_builder::get_reverse_ast(const ast &tree) {
    std::stack<std::shared_ptr<ast>> subtree;
    for (const auto &iter: tree) {
        auto type = iter.get_node_type();
        if (type == node::node_type::leaf) {
            subtree.emplace(std::make_shared<ast>(std::make_shared<node>(iter.get_label())));
        }
        if (type == node::node_type::concat || type == node::node_type::alternation) {
            std::shared_ptr<ast> right = subtree.top();
            subtree.pop();
            std::shared_ptr<ast> left = subtree.top();
            subtree.pop();
            if (type == node::node_type::concat && right->get_root().get_label() == end_symbol)
                std::swap(right, left);
            subtree.emplace(std::make_shared<ast>(type, *right, *left));
        }
        if (type == node::node_type::star || type == node::node_type::optional) {
            std::shared_ptr<ast> sub_tree = subtree.top();
            subtree.pop();
            subtree.emplace(std::make_shared<ast>(type, *sub_tree));
        }
    }
    return *subtree.top();
}