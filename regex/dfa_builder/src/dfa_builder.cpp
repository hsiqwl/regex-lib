#include "dfa_builder.h"

void dfa_builder::calc_first_pos(node_ptr tree_node) {
    switch (tree_node->get_node_type()) {
        case node::node_type::leaf: {
            if (tree_node->get_label() != '$')
                first_pos_table.emplace(tree_node, std::unordered_set<size_t>{leaf_counter});
            else
                first_pos_table.emplace(tree_node, std::unordered_set<size_t>{});
            if (tree_node->get_label() != '#' && tree_node->get_label() != '$') {
                alphabet.insert(tree_node->get_label());
            }
            if (tree_node->get_label() != '$')
                char_to_pos_table[tree_node->get_label()].insert(leaf_counter);
            return;
        }
        case node::node_type::concat: {
            calc_first_pos_for_concat(tree_node);
            return;
        }
        case node::node_type::alternation: {
            calc_first_pos_for_alternation(tree_node);
            return;
        }
        default: {
            calc_first_pos_for_star(tree_node);
            return;
        }
    }
}

void dfa_builder::calc_last_pos(node_ptr tree_node) {
    switch (tree_node->get_node_type()) {
        case node::node_type::leaf: {
            if(tree_node->get_label() != '$')
                last_pos_table.emplace(tree_node, std::unordered_set<size_t>{leaf_counter++});
            return;
        }
        case node::node_type::concat: {
            calc_last_pos_for_concat(tree_node);
            return;
        }
        case node::node_type::alternation: {
            calc_last_pos_for_alternation(tree_node);
            return;
        }
        default: {
            calc_last_pos_for_star(tree_node);
            return;
        }
    }
}

void dfa_builder::calc_first_pos_for_alternation(node_ptr tree_node) {
    auto& left_first_pos = first_pos_table[tree_node->get_left_child().get()];
    auto& right_first_pos = first_pos_table[tree_node->get_right_child().get()];
    auto union_set = position_set{left_first_pos.begin(), left_first_pos.end()};
    union_set.merge(position_set{right_first_pos.begin(), right_first_pos.end()});
    first_pos_table.emplace(tree_node, union_set);
}

void dfa_builder::calc_first_pos_for_star(node_ptr tree_node) {
    first_pos_table.emplace(tree_node, first_pos_table[tree_node->get_left_child().get()]);
}

void dfa_builder::calc_first_pos_for_concat(node_ptr tree_node) {
    first_pos_table.emplace(tree_node, first_pos_table[tree_node->get_left_child().get()]);
    if (tree_node->get_left_child()->is_nullable()) {
        auto& right_first_pos = first_pos_table[tree_node->get_right_child().get()];
        first_pos_table[tree_node].merge(position_set{right_first_pos.begin(), right_first_pos.end()});
    }
}

void dfa_builder::calc_last_pos_for_alternation(node_ptr tree_node) {
    auto left_last_pos = last_pos_table[tree_node->get_left_child().get()];
    auto right_last_pos = last_pos_table[tree_node->get_right_child().get()];
    left_last_pos.merge(right_last_pos);
    last_pos_table.emplace(tree_node, left_last_pos);
}

void dfa_builder::calc_last_pos_for_star(node_ptr tree_node) {
    last_pos_table.emplace(tree_node, last_pos_table[tree_node->get_left_child().get()]);
}

void dfa_builder::calc_last_pos_for_concat(node_ptr tree_node) {
    last_pos_table.emplace(tree_node, last_pos_table[tree_node->get_right_child().get()]);
    if (tree_node->get_right_child()->is_nullable()) {
        auto& left_last_pos = last_pos_table[tree_node->get_left_child().get()];
        last_pos_table[tree_node].merge(position_set{left_last_pos.begin(), left_last_pos.end()});
    }
}

void dfa_builder::calc_follow_pos(node_ptr tree_node) {
    if (tree_node->get_node_type() == node::node_type::concat) {
        for (auto pos: last_pos_table[tree_node->get_left_child().get()]) {
            auto& right_first_pos = first_pos_table[tree_node->get_right_child().get()];
            follow_positions[pos - 1].merge(position_set{right_first_pos.begin(), right_first_pos.end()});
        }
    } else if (tree_node->get_node_type() == node::node_type::star) {
        for (auto pos: last_pos_table[tree_node]) {
            auto& first_pos = first_pos_table[tree_node];
            follow_positions[pos - 1].merge(position_set{first_pos.begin(), first_pos.end()});
        }
    }
}


dfa_builder::dfa_builder(const ast &tree): root(tree.get_root()) {
    leaf_counter = 1;
    pre_build(tree);
}

void dfa_builder::pre_build(const ast &tree) {
    for (auto &tree_node: tree) {
        calc_first_pos(const_cast<node_ptr>(&tree_node));
        calc_last_pos(const_cast<node_ptr>(&tree_node));
    }
    follow_positions.resize(leaf_counter - 1);
    for(auto &tree_node: tree){
        calc_follow_pos(const_cast<node_ptr>(&tree_node));
    }
    build_group_manager(tree);
}

nfa dfa_builder::get_nfa_simulator() {
    std::unordered_set<size_t> initial_pos = {first_pos_table[const_cast<node_ptr>(&root)].begin(),
                                              first_pos_table[const_cast<node_ptr>(&root)].end()};
    std::unordered_set<size_t> last_pos{char_to_pos_table[end_symbol].begin(), char_to_pos_table[end_symbol].end()};
    return {std::move(follow_positions), std::move(char_to_pos_table), std::move(initial_pos), std::move(last_pos)};
}

dfa dfa_builder::build() {
    bm_type combination_to_states;
    std::vector<std::shared_ptr<state>> unmarked_states;
    std::shared_ptr<state> error_state = make_error_state();
    std::set<size_t> initial_positions{first_pos_table[const_cast<node_ptr>(&root)].begin(),
                                       first_pos_table[const_cast<node_ptr>(&root)].end()};
    bool acceptance = initial_positions.contains(*char_to_pos_table[end_symbol].begin());
    std::shared_ptr<state> initial_state = std::make_shared<state>(acceptance);
    initialize_state(initial_state, error_state);
    unmarked_states.emplace_back(initial_state);
    combination_to_states.insert(bm_type::value_type(initial_positions, initial_state));
    auto states = construct_states(unmarked_states, combination_to_states, error_state);
    states.emplace_back(error_state);
    dfa automaton = construct_dfa_from_states(std::move(states));
    dfa_minimizer minimizer;
    minimizer.minimize(automaton);
    manager.set_engine(get_nfa_simulator());
    return automaton;
}

dfa dfa_builder::construct_dfa_from_states(std::vector<std::shared_ptr<state>> &&states) {
    dfa automaton;
    automaton.initial_state = states[0];
    automaton.curr_state = states[0];
    automaton.states = std::move(states);
    for (auto &state: automaton.states) {
        if (state->is_accepting()) {
            automaton.accepting_states.emplace_back(state);
        }
    }
    return automaton;
}

std::vector<std::shared_ptr<state>> dfa_builder::construct_states(std::vector<std::shared_ptr<state>> &unmarked_states,
                                   dfa_builder::bm_type &combination_to_states, const std::shared_ptr<state>& error_state) {
    std::vector<std::shared_ptr<state>> states;
    while (!unmarked_states.empty()) {
        std::shared_ptr<state> curr_state = unmarked_states[0];
        unmarked_states.erase(unmarked_states.begin());
        states.emplace_back(curr_state);
        for (char c: alphabet) {
            auto combination = get_positions_for_input_char(c, combination_to_states.right.find(curr_state)->second);
            if (!combination.empty()) {
                if (combination_to_states.left.find(combination) == combination_to_states.left.end()) {
                    bool acceptance = combination.contains(*char_to_pos_table[end_symbol].begin());
                    std::shared_ptr<state> new_state = std::make_shared<state>(acceptance);
                    initialize_state(new_state, error_state);
                    combination_to_states.insert(bm_type::value_type(combination, new_state));
                    unmarked_states.emplace_back(new_state);
                }
                curr_state->add_transition(c, combination_to_states.left.find(combination)->second);
            }
        }
    }
    return states;
}

std::set<size_t> dfa_builder::get_positions_for_input_char(char c, const std::set<size_t>& set) {
    std::set<size_t> result;
    for(auto pos: char_to_pos_table[c]) {
        if (set.contains(pos)) {
            result.insert(follow_positions[pos - 1].begin(), follow_positions[pos - 1].end());
        }
    }
    return result;
}

std::shared_ptr<state> dfa_builder::make_error_state() {
    std::shared_ptr<state> error_state = std::make_shared<state>(false);
    error_state->declare_as_error_state();
    for(auto i = 0; i < 256; ++i){
        error_state->add_transition(i, error_state);
    }
    return error_state;
}

void dfa_builder::initialize_state(std::shared_ptr<state> &st, const std::shared_ptr<state>& error_state) {
    for(auto i = 0; i < 256; ++i)
        st->add_transition(i, error_state);
}

void dfa_builder::build_group_manager(const ast &tree) {
    for (auto &tree_node: tree) {
        auto &group_info = tree_node.get_group_info();
        auto first = group_info.get_tracked_groups().first;
        auto last = group_info.get_tracked_groups().second;
        while (first != last) {
            std::unordered_set<size_t> &starting_positions(first_pos_table[const_cast<node_ptr>(&tree_node)]);
            std::unordered_set<size_t> &ending_positions(last_pos_table[const_cast<node_ptr>(&tree_node)]);
            size_t pos_of_end = *char_to_pos_table[end_symbol].begin();
            group_tracker tracker(starting_positions, ending_positions, group_info.group_is_repetitive(), pos_of_end);
            manager.manage_group(*first, std::move(tracker));
            ++first;
        }
    }
}

group_manager dfa_builder::get_group_manager() const noexcept {
    return manager;
}