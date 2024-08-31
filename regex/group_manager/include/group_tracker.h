#ifndef REGEX_GROUP_TRACKER_H
#define REGEX_GROUP_TRACKER_H
#include <unordered_set>
#include <vector>
#include <string>
#include <algorithm>

class group_tracker{
private:
    std::unordered_set<size_t> tracked_states;

    std::unordered_set<size_t> starting_states;

    std::unordered_set<size_t> ending_states;

    bool is_repetitive = true;

    std::string substring;

    bool exiting_capturing_state(size_t from, size_t to);

    bool transitioning_within_capturing_states(size_t from, size_t to);

    std::string find_longest_substring(const std::vector<std::pair<size_t, char>>& path);

public:
    group_tracker() = default;

    group_tracker(group_tracker&& first, group_tracker&& second);

    group_tracker(const std::unordered_set<size_t>& starting, const std::unordered_set<size_t>& ending, bool is_repetitive, size_t pos_of_end);

    void set_substring(const std::vector<std::vector<std::pair<size_t, char>>>& paths);

    std::string get_substring() const noexcept;

    void reset_substring();
};
#endif //REGEX_GROUP_TRACKER_H
