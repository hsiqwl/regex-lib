#ifndef REGEX_NFA_H
#define REGEX_NFA_H
#include <vector>
#include <unordered_set>
#include <array>
#include <algorithm>
#include <stack>
#include "trie.h"

class nfa{
private:
    std::vector<std::unordered_set<size_t>> follow_pos;

    std::array<std::unordered_set<size_t>, 256> char_to_pos;

    std::unordered_set<size_t> initial_pos;

    std::unordered_set<size_t> curr_pos;

    std::unordered_set<size_t> last_pos;

    std::vector<std::shared_ptr<trie_node>> leafs;

    std::shared_ptr<trie_node> root;

    char get_char_for_position(size_t pos);

    void reset_trie();
public:
    nfa() = default;

    nfa(std::vector<std::unordered_set<size_t>>&& follow_pos,
        std::array<std::unordered_set<size_t>, 256>&& char_to_pos,
        std::unordered_set<size_t>&& initial_pos, std::unordered_set<size_t>&& last_pos);

    void consume_input(char c);

    bool is_in_accepting_state() const noexcept;

    void reset() noexcept;

    std::vector<std::vector<std::pair<size_t, char>>> get_paths_to_end_state() const noexcept;
};

#endif //REGEX_NFA_H
