cmake_minimum_required(VERSION 3.27)
project(regex_engine)

set(CMAKE_CXX_STANDARD 23)


add_library(regex_engine STATIC src/regex.cpp)
add_compile_options(-g -fsanitize=address)
add_link_options(-fsanitize=address)
target_include_directories(regex_engine PUBLIC include)
target_link_libraries(regex_engine dfa regex_tokenizer dfa_builder ast_builder dfa_to_regex)