#ifndef REGEX_TERMINAL_INFO_H
#define REGEX_TERMINAL_INFO_H
#include <string>

class terminal_info{
private:
    char range_min;

    char range_max;

    std::string singles;

    bool defines_character_class;

public:
    terminal_info() = default;

    terminal_info(char c);

    terminal_info(char range_min, char range_max, std::string singles = {});

    bool is_character_class() const noexcept;

    char get_range_min() const noexcept;

    char get_range_max() const noexcept;

    const std::string& get_singles() const noexcept;
};

#endif //REGEX_TERMINAL_INFO_H
