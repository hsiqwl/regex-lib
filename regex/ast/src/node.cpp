#include "node.h"

node::node(char label): label(label), nullable(false) {
    if(label == '$'){
        nullable = true;
    }
    type = node_type::leaf;
}

node::node(node::node_type n_type, const std::shared_ptr<node>& child) {
    if (n_type == node_type::star || n_type == node_type::optional) {
        left_child = child;
        type = n_type;
        nullable = true;
        label = (n_type == node_type::star) ? '*' : '?';
    } else {
        throw std::logic_error("bad node type");
    }
}

node::node(node::node_type n_type, const std::shared_ptr<node>& left, const std::shared_ptr<node>& right) {
    if (n_type == node_type::leaf || n_type == node_type::star || n_type == node_type::optional) {
        throw std::logic_error("bad node type");
    }
    type = n_type;
    left_child = left;
    right_child = right;
    if (type == node_type::concat) {
        nullable = left_child->nullable && right_child->nullable;
        label = '.';
    } else {
        nullable = left_child->nullable || right_child->nullable;
        label = '|';
    }
}

node::node_type node::get_node_type() const noexcept {
    return type;
}

const std::shared_ptr<node>& node::get_right_child() const noexcept {
    return right_child;
}

const std::shared_ptr<node>& node::get_left_child() const noexcept {
    return left_child;
}

char node::get_label() const noexcept {
    return label;
}

const std::shared_ptr<node>& node::get_next() const noexcept {
    return next;
}

bool node::is_nullable() const noexcept {
    return nullable;
}

void node::set_group_info(const group_info &info) {
    tracked_groups = info;
}

const group_info &node::get_group_info() const noexcept {
    return tracked_groups;
}

