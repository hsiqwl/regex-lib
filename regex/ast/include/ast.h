#ifndef REGEX_AST_H
#define REGEX_AST_H
#include "node.h"
#include <stack>
#include <functional>
#include <ast_iterator.h>


class ast{
public:
    ast(const std::shared_ptr<node>& new_root);

    ast(node::node_type root_type, ast& left, ast& right);

    ast(node::node_type root_type, ast& sub_tree);

    [[nodiscard]] ast get_deep_copy() const; //copy constructor

    typedef ast_iterator<false> iterator;

    typedef ast_iterator<true> const_iterator;

    const node& get_root() const noexcept;

    node& get_root();

    iterator begin() noexcept;

    const_iterator begin() const noexcept;

    iterator end() noexcept;

    const_iterator end() const noexcept;

    const_iterator cbegin() const noexcept;

    const_iterator cend() const noexcept;

private:
    std::shared_ptr<node> root;

    [[nodiscard]] std::shared_ptr<node> find_most_left() const noexcept;

};


#endif //REGEX_AST_H
