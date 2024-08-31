#include "match_results.h"

match_result::iterator match_result::begin() {
    return {submatch.begin()};
}

match_result::iterator match_result::end() {
    return {submatch.end()};
}

match_result::const_iterator match_result::cend() {
    return {submatch.cend()};
}

match_result::const_iterator match_result::cbegin() {
    return {submatch.cbegin()};
}

std::string &match_result::operator [] (int index) {
    return submatch[index];
}

const std::string &match_result::operator [] (int index) const {
    return submatch[index];
}

match_result::match_result(regex &executor, std::string_view string) {
    for (char c: string) {
        executor.manager.nfa_engine.consume_input(c);
    }
    if (executor.manager.nfa_engine.is_in_accepting_state()) {
        executor.manager.set_substrings();
        submatch.resize(executor.manager.groups.size());
        for (auto &group: executor.manager.groups) {
            submatch[group.first] = group.second.get_substring();
        }
    }
    executor.manager.reset();
}

size_t match_result::size() const noexcept {
    return submatch.size();
}