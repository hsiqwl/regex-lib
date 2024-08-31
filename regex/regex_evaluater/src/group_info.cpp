#include "group_info.h"

void group_info::add_group(size_t group_number) {
    tracked_groups.insert(tracked_groups.begin(), group_number);
}

std::pair<group_info::const_iterator, group_info::const_iterator> group_info::get_tracked_groups() const noexcept {
    return {tracked_groups.cbegin(), tracked_groups.cend()};
}

group_info::group_info(bool repetitive) {
    is_repetitive = repetitive;
}

void group_info::set_repetitive() noexcept {
    is_repetitive = true;
}

bool group_info::group_is_repetitive() const noexcept {
    return is_repetitive;
}