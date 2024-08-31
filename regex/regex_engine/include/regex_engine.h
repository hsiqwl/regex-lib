#ifndef REGEX_REGEX_ENGINE_H
#define REGEX_REGEX_ENGINE_H
#include "regex_tokenizer.h"
#include "ast_builder.h"
#include "dfa_builder.h"
#include "dfa_to_regex.h"

class match_result;

class regex{
public:
    friend match_result;

    regex(std::string_view expression);

    bool match(std::string_view string);

    regex get_reverse_language() const;

    [[nodiscard]] regex get_complemented_language() const;

    std::string get_initial_regex() const noexcept;

private:
    regex() = default;

    group_manager manager;

    dfa engine;

    bool is_complemented = false;
};

#endif //REGEX_REGEX_ENGINE_H
