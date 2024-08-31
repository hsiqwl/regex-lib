cmake_minimum_required(VERSION 3.27)
project(dfa_to_regex)

set(CMAKE_CXX_STANDARD 23)

add_library(dfa_to_regex STATIC src/dfa_to_regex.cpp)
target_include_directories(dfa_to_regex PUBLIC include)
target_link_libraries(dfa_to_regex dfa)