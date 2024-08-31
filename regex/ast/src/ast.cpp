#include "ast.h"

ast::ast(const std::shared_ptr<node>& new_root) {
    root = new_root;
}

ast::ast(node::node_type root_type, ast &sub_tree) {
    if (root_type == node::node_type::star || root_type == node::node_type::optional) {
        root = std::make_shared<node>(root_type, sub_tree.root);
    }
    sub_tree.root->next = root;
}

ast::ast(node::node_type root_type, ast &left, ast &right) {
    if(root_type == node::node_type::concat || root_type == node::node_type::alternation){
        root = std::make_shared<node>(root_type, left.root, right.root);
    }
    left.root->next = right.find_most_left();
    right.root->next = root;
}

std::shared_ptr<node> ast::find_most_left() const noexcept {
    std::shared_ptr<node> most_left = root;
    while(most_left->left_child != nullptr){
        most_left = most_left->left_child;
    }
    return most_left;
}

ast ast::get_deep_copy() const {
    std::stack<std::shared_ptr<ast>> subtree;
    auto iter = begin();
    while (iter != end()) {
        auto type = iter->type;
        if (type == node::node_type::leaf) {
            subtree.emplace(std::make_shared<ast>(std::make_shared<node>(iter->label)));
        }
        if (type == node::node_type::concat || type == node::node_type::alternation) {
            std::shared_ptr<ast> right = subtree.top();
            subtree.pop();
            std::shared_ptr<ast> left = subtree.top();
            subtree.pop();
            subtree.emplace(std::make_shared<ast>(type, *left, *right));
        }
        if (type == node::node_type::star || type == node::node_type::optional) {
            std::shared_ptr<ast> sub_tree = subtree.top();
            subtree.pop();
            subtree.emplace(std::make_shared<ast>(type, *sub_tree));
        }
        subtree.top()->root->set_group_info(iter->tracked_groups);
        ++iter;
    }
    return *subtree.top();
}

ast::iterator ast::begin() noexcept {
    return {find_most_left().get()};
}

ast::const_iterator ast::cbegin() const noexcept {
    return {find_most_left().get()};
}

ast::const_iterator ast::begin() const noexcept {
    return {find_most_left().get()};
}

ast::iterator ast::end() noexcept {
    return {root->next.get()};
}

ast::const_iterator ast::end() const noexcept {
    return {root->next.get()};
}

ast::const_iterator ast::cend() const noexcept {
    return {root->next.get()};
}

const node &ast::get_root() const noexcept {
    return *root;
}

node &ast::get_root() {
    return *root;
}
