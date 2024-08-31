#include "terminal_info.h"

terminal_info::terminal_info(char c) {
    range_min = c;
    range_max = c;
    defines_character_class = false;
}

terminal_info::terminal_info(char range_min, char range_max, std::string singles):
    range_max(range_max), range_min(range_min), singles(std::move(singles)) {
    defines_character_class  = true;
}

bool terminal_info::is_character_class() const noexcept {
    return defines_character_class;
}

const std::string &terminal_info::get_singles() const noexcept {
    return singles;
}

char terminal_info::get_range_min() const noexcept {
    return range_min;
}

char terminal_info::get_range_max() const noexcept {
    return range_max;
}