#ifndef REGEX_MATCH_RESULTS_H
#define REGEX_MATCH_RESULTS_H
#include "regex_engine.h"

class match_result{
private:
    std::vector<std::string> submatch;

public:
    typedef std::vector<std::string>::const_iterator const_iterator;

    typedef std::vector<std::string>::iterator iterator;

    match_result(regex& executor, std::string_view string);

    iterator begin();

    iterator end();

    const_iterator cbegin();

    const_iterator cend();

    std::string& operator [] (int index);

    const std::string& operator [] (int index) const;

    size_t size() const noexcept;
};


#endif //REGEX_MATCH_RESULTS_H
