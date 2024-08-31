#ifndef REGEX_AST_ITERATOR_H
#define REGEX_AST_ITERATOR_H
#include <iterator>
#include <utility>
#include "node.h"

class ast;

template <bool is_const>
class ast_iterator {
private:
    typedef std::conditional_t<is_const, const node, node> *elem_ptr;

    elem_ptr node_ptr;

    ast_iterator(elem_ptr ptr) : node_ptr(ptr) {}

    friend class ast;

    friend ast_iterator<!is_const>;

public:
    typedef std::ptrdiff_t difference_type;

    typedef node val_type;

    typedef std::conditional_t<is_const, const val_type, val_type> *pointer;

    typedef std::conditional_t<is_const, const val_type, val_type> &reference;

    template<bool other_const>
    ast_iterator(const ast_iterator<is_const> &other_iter) noexcept requires (is_const >= other_const) {
        node_ptr = other_iter.node_ptr;
    }

    template<bool other_const>
    ast_iterator(ast_iterator<is_const> &&other_iter) noexcept requires (is_const >= other_const) {
        node_ptr = std::exchange(other_iter.node_ptr, nullptr);
    }

    template<bool other_const>
    ast_iterator& operator = (const ast_iterator<is_const> &other_iter) noexcept requires (is_const >= other_const) {
        node_ptr = other_iter.node_ptr;
        return *this;
    }

    template<bool other_const>
    ast_iterator& operator = (ast_iterator<other_const> &&other_iter) noexcept requires (is_const >= other_const) {
        node_ptr = std::exchange(other_iter.node_ptr, nullptr);
        return *this;
    }

    template<bool other_const>
    bool operator==(const ast_iterator<other_const> &other_iter) const noexcept {
        return node_ptr == other_iter.node_ptr;
    }

    ast_iterator &operator++() {
        node_ptr = node_ptr->get_next().get();
        return *this;
    }

    ast_iterator operator++(int) {
        elem_ptr prev = node_ptr;
        ++this;
        return ast_iterator(prev);
    }

    reference operator*() const noexcept {
        return *node_ptr;
    }

    pointer operator->() const noexcept {
        return node_ptr;
    }
};



#endif //REGEX_AST_ITERATOR_H
