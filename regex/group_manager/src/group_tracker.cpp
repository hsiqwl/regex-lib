#include "group_tracker.h"

group_tracker::group_tracker(const std::unordered_set<size_t> &starting, const std::unordered_set<size_t> &ending,
                             bool is_repetitive, size_t pos_of_end): is_repetitive(is_repetitive) {
    size_t start_min = *std::min_element(starting.begin(), starting.end());
    size_t end_max = *std::max_element(ending.begin(), ending.end());
    for(auto i = start_min; i <= end_max; ++i){
        tracked_states.insert(i);
    }
    starting_states = starting;
    ending_states = ending;
}

group_tracker::group_tracker(group_tracker &&first, group_tracker &&second) {
    starting_states = std::move(first.starting_states);
    starting_states.merge(std::move(second.starting_states));
    ending_states = std::move(first.ending_states);
    ending_states.merge(std::move(second.ending_states));
    size_t start_min = *std::min_element(starting_states.begin(), starting_states.end());
    size_t end_max = *std::max_element(ending_states.begin(), ending_states.end());
    for (auto i = start_min; i <= end_max; ++i) {
        tracked_states.insert(i);
    }
    is_repetitive = first.is_repetitive && second.is_repetitive;
}

std::string group_tracker::get_substring() const noexcept {
    return substring;
}

/*bool group_tracker::entering_capturing_state(size_t from, size_t to) {
    bool first_predicate =  !tracked_states.contains(from);
    bool second_predicate = ending_states.contains(from);
    return (first_predicate || second_predicate) && starting_states.contains(to);
}*/

bool group_tracker::exiting_capturing_state(size_t from, size_t to) {
    return ending_states.contains(from) && !tracked_states.contains(to);
}

bool group_tracker::transitioning_within_capturing_states(size_t from, size_t to) {
    return tracked_states.contains(from) && tracked_states.contains(to);
}

std::string group_tracker::find_longest_substring(const std::vector<std::pair<size_t, char>> &path) {
    std::string char_path;
    for(auto iter = path.begin() + 1; iter != path.end(); ++iter) {
        size_t from = (iter - 1)->first;
        size_t to = iter->first;
        if(transitioning_within_capturing_states(from, to) || exiting_capturing_state(from, to)){
            if(starting_states.contains(from) && !is_repetitive){
                char_path.clear();
            }
            char_path.push_back((iter - 1)->second);
        }
    }
    return char_path;
}

void group_tracker::set_substring(const std::vector<std::vector<std::pair<size_t, char>>>& paths) {
    for (auto &path: paths) {
        std::string char_path = find_longest_substring(path);
        if (char_path.size() > substring.size()) {
            substring = std::move(char_path);
        }
    }
}

void group_tracker::reset_substring() {
    substring.clear();
}