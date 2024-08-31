#include "group_manager.h"

void group_manager::manage_group(size_t group_number, group_tracker &&tracker) {
    groups[group_number] = group_tracker(std::move(groups[group_number]), std::move(tracker));
}

std::string group_manager::get_group_substring(size_t group_number) {
    return groups.find(group_number)->second.get_substring();
}

void group_manager::consume_input(char input) {
    nfa_engine.consume_input(input);
}

void group_manager::set_engine(nfa &&engine) {
    nfa_engine = std::move(engine);
}

void group_manager::set_substrings() {
    if (nfa_engine.is_in_accepting_state()) {
        for (auto &group: groups) {
            auto paths = nfa_engine.get_paths_to_end_state();
            group.second.set_substring(paths);
        }
    }
}

void group_manager::reset() {
    nfa_engine.reset();
    for (auto &group: groups) {
        group.second.reset_substring();
    }
}