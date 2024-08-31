#include "trie.h"

trie_node::trie_node(size_t branch_id, char label): branch_id(branch_id), label(label) {}


size_t trie_node::get_branch_id() const noexcept {
    return branch_id;
}

void trie_node::add_branch(const std::shared_ptr<trie_node>& added_branch) {
    branches[added_branch->branch_id] = added_branch;
}

void trie_node::erase_branch(size_t branch_num) {
    branches.erase(branch_num);
}

void trie_node::remove_itself_from_parent_branch() {
    parent_branch.lock()->erase_branch(branch_id);
}

void trie_node::set_parent_branch(const std::shared_ptr<trie_node> &parent) {
    parent_branch = parent;
}

std::shared_ptr<trie_node> trie_node::get_parent_branch() const {
    return parent_branch.lock();
}

char trie_node::get_label() const noexcept {
    return label;
}

void trie_node::erase_all_branches() {
    for(auto& branch: branches){
        branch.second->erase_all_branches();
    }
    branches.clear();
}