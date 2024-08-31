#include "regex_engine.h"

regex::regex(std::string_view expression) {
    regex_tokenizer tokenizer(std::string{expression});
    std::pair<regex_tokenizer::token_iterator, regex_tokenizer::token_iterator> iterators = std::make_pair(tokenizer.get_token_sequence().begin(), tokenizer.get_token_sequence().end());
    ast tree = ast_builder::tokens_to_ast(iterators);
    dfa_builder builder(tree);
    engine = builder.build();
    manager = builder.get_group_manager();
}

bool regex::match(std::string_view string) {
    for (char c: string) {
        engine.consume_input(c);
        if (engine.is_in_error_state()) {
            engine.reset();
            return is_complemented;
        }
    }
    bool matches = engine.is_in_accepting_state() ^ is_complemented;
    engine.reset();
    return matches;
}

regex regex::get_complemented_language() const {
    regex result(*this);
    result.is_complemented = true;
    return result;
}

std::string regex::get_initial_regex() const noexcept {
    return dfa_to_regex::convert_dfa_to_regex(engine);
}

regex regex::get_reverse_language() const {
    regex reverse;
    std::string initial_regex = get_initial_regex();
    regex_tokenizer tokenizer(initial_regex);
    std::pair<regex_tokenizer::token_iterator, regex_tokenizer::token_iterator> iterators = std::make_pair(
            tokenizer.get_token_sequence().begin(), tokenizer.get_token_sequence().end());
    ast tree = ast_builder::tokens_to_ast(iterators);
    tree = ast_builder::get_reverse_ast(tree);
    dfa_builder builder(tree);
    reverse.engine = builder.build();
    reverse.is_complemented = is_complemented;
    return reverse;
}