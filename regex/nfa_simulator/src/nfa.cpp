#include "nfa.h"

nfa::nfa(std::vector<std::unordered_set<size_t>> &&follow_positions,
         std::array<std::unordered_set<size_t>, 256> &&char_to_positions,
         std::unordered_set<size_t> &&initial_positions,
         std::unordered_set<size_t>&& last_positions) {
    follow_pos = std::move(follow_positions);
    char_to_pos = std::move(char_to_positions);
    initial_pos = std::move(initial_positions);
    last_pos = std::move(last_positions);
    curr_pos = initial_pos;
    root = std::make_shared<trie_node>(0, '\0');
    for(auto pos: initial_pos){
        std::shared_ptr<trie_node> new_branch = std::make_shared<trie_node>(pos, get_char_for_position(pos));
        new_branch->set_parent_branch(root);
        root->add_branch(new_branch);
        leafs.emplace_back(new_branch);
    }
}

void nfa::reset() noexcept {
    curr_pos = initial_pos;
    reset_trie();
}

bool nfa::is_in_accepting_state() const noexcept {
    return std::find_first_of(curr_pos.begin(), curr_pos.end(), last_pos.begin(), last_pos.end()) != curr_pos.end();
}

void nfa::consume_input(char c) {
    std::unordered_set<size_t> corresponding_curr_states;
    std::unordered_set<size_t> next_pos;
    for (auto pos: curr_pos) {
        if (char_to_pos[c].contains(pos)) {
            corresponding_curr_states.insert(pos);
        }
    }
    std::vector<std::shared_ptr<trie_node>> new_leafs;
    for (size_t i = 0; i < leafs.size(); ++i) {
        std::shared_ptr<trie_node>& leaf = leafs[i];
        if (!corresponding_curr_states.contains(leaf->get_branch_id())) {
            leaf->remove_itself_from_parent_branch();
            leafs.erase(leafs.begin() + i);
            --i;
        } else {
            for (auto pos: follow_pos[leaf->get_branch_id() - 1]) {
                next_pos.insert(pos);
                new_leafs.emplace_back(std::make_shared<trie_node>(pos, get_char_for_position(pos)));
                new_leafs.back()->set_parent_branch(leaf);
                leaf->add_branch(new_leafs.back());
            }
        }
    }
    leafs = std::move(new_leafs);
    curr_pos = std::move(next_pos);
}

std::vector<std::vector<std::pair<size_t, char>>> nfa::get_paths_to_end_state() const noexcept {
    std::vector<std::vector<std::pair<size_t, char>>> paths;
    for(auto leaf: leafs){
        if(leaf->get_branch_id() == *last_pos.begin()) {
            paths.emplace_back();
            while (leaf->get_branch_id() != 0) {
                paths.back().insert(paths.back().begin(), {leaf->get_branch_id(), leaf->get_label()});
                leaf = leaf->get_parent_branch();
            }
        }
    }
    return paths;
}


char nfa::get_char_for_position(size_t pos) {
    for (auto iter = char_to_pos.begin(); iter != char_to_pos.end(); ++iter) {
        if (iter->contains(pos))
            return (char) (std::distance(char_to_pos.begin(), iter));
    }
}

void nfa::reset_trie() {
    root->erase_all_branches();
    leafs.clear();
    for (auto pos: initial_pos) {
        std::shared_ptr<trie_node> new_branch = std::make_shared<trie_node>(pos, get_char_for_position(pos));
        new_branch->set_parent_branch(root);
        root->add_branch(new_branch);
        leafs.emplace_back(new_branch);
    }
}