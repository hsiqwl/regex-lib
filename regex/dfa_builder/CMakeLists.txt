cmake_minimum_required(VERSION 3.27)
project(dfa_builder)

set(CMAKE_CXX_STANDARD 23)
add_compile_options(-g)

add_library(dfa_builder STATIC src/dfa_builder.cpp)
add_compile_options(-g -fsanitize=address)
add_link_options(-fsanitize=address)
target_include_directories(dfa_builder PUBLIC include)
target_link_libraries(dfa_builder ast dfa_minimizer dfa nfa_simulator group_tracker)
