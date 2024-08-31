#ifndef REGEX_TRIE_H
#define REGEX_TRIE_H
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <memory>

class trie_node{
private:
    std::unordered_map<size_t, std::shared_ptr<trie_node>> branches;

    size_t branch_id;

    char label;

    std::weak_ptr<trie_node> parent_branch;

public:
    trie_node(size_t branch_id, char label);

    void add_branch(const std::shared_ptr<trie_node>& added_branch);

    void set_parent_branch(const std::shared_ptr<trie_node>& parent);

    std::shared_ptr<trie_node> get_parent_branch() const;

    [[nodiscard]] size_t get_branch_id() const noexcept;

    void remove_itself_from_parent_branch();

    void erase_branch(size_t branch_num);

    char get_label() const noexcept;

    void erase_all_branches();
};

#endif //REGEX_TRIE_H
