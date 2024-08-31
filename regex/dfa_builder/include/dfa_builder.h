#ifndef REGEX_DFA_BUILDER_H
#define REGEX_DFA_BUILDER_H
#include <array>
#include <memory>
#include <set>
#include <unordered_set>
#include <functional>
#include <algorithm>
#include <ranges>
#include "dfa.h"
#include "ast.h"
#include <boost/bimap.hpp>
#include "nfa.h"
#include "dfa_minimizer.h"
#include "group_manager.h"
#include <fstream>

namespace std {
    template<>
    struct hash<std::set<unsigned long>> {
        size_t operator()(const std::set<unsigned long> &set) const noexcept {
            size_t resulting_hash;
            for (auto elem: set) {
                resulting_hash += std::hash<unsigned long>()(elem);
            }
            return resulting_hash;
        }
    };
}

class dfa_builder{
public:
    dfa_builder(const ast& tree);

    dfa build();

    group_manager get_group_manager() const noexcept;

private:
    const char end_symbol = '#';

    size_t leaf_counter;

    typedef node* node_ptr;

    typedef std::unordered_set<size_t> position_set;

    typedef boost::bimap<std::set<size_t>, std::shared_ptr<state>> bm_type;

    const node& root;

    std::unordered_map<node_ptr, position_set> first_pos_table;

    std::unordered_map<node_ptr, position_set> last_pos_table;

    std::vector<position_set> follow_positions;

    std::unordered_set<char> alphabet;

    std::array<position_set, 256> char_to_pos_table;

    group_manager manager;

    nfa get_nfa_simulator();

    void build_group_manager(const ast& tree);

    void pre_build(const ast& tree);

    void calc_first_pos(node_ptr tree_node);

    void calc_last_pos(node_ptr tree_node);

    void calc_follow_pos(node_ptr tree_node);

    void calc_first_pos_for_alternation(node_ptr tree_node);

    void calc_first_pos_for_concat(node_ptr tree_node);

    void calc_first_pos_for_star(node_ptr tree_node);

    void calc_last_pos_for_alternation(node_ptr tree_node);

    void calc_last_pos_for_concat(node_ptr tree_node);

    void calc_last_pos_for_star(node_ptr tree_node);

    std::set<size_t> get_positions_for_input_char(char c, const std::set<size_t>& set);

    std::vector<std::shared_ptr<state>> construct_states(std::vector<std::shared_ptr<state>>& unmarked_states, bm_type& combination_to_states, const std::shared_ptr<state>& state);

    dfa construct_dfa_from_states(std::vector<std::shared_ptr<state>>&& states);

    static std::shared_ptr<state> make_error_state();

    static void initialize_state(std::shared_ptr<state>& st, const std::shared_ptr<state>& error_state);
};

#endif //REGEX_DFA_BUILDER_H
