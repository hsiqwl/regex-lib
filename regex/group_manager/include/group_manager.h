#ifndef REGEX_GROUP_MANAGER_H
#define REGEX_GROUP_MANAGER_H
#include "group_tracker.h"
#include "nfa.h"
#include <unordered_map>

class match_result;

class group_manager{
private:
    std::unordered_map<size_t, group_tracker> groups;

    nfa nfa_engine;
public:
    friend class match_result;

    group_manager() = default;

    void set_engine(nfa&& engine);

    void consume_input(char input);

    std::string get_group_substring(size_t group_number);

    void manage_group(size_t group_number, group_tracker&& tracker);

    void set_substrings();

    void reset();
};

#endif //REGEX_GROUP_MANAGER_H
