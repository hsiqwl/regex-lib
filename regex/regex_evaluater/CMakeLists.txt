cmake_minimum_required(VERSION 3.27)
project(regex_tokenizer)

set(CMAKE_CXX_STANDARD 23)
add_compile_options(-g)

add_library(token STATIC src/token.cpp
        src/terminal_info.cpp
        src/operator_info.cpp
        src/group_info.cpp)
target_include_directories(token PUBLIC include)

add_library(regex_tokenizer STATIC src/regex_tokenizer.cpp)
add_compile_options(-g -fsanitize=address)
add_link_options(-fsanitize=address)
target_include_directories(regex_tokenizer PUBLIC include)
target_link_libraries(regex_tokenizer token)