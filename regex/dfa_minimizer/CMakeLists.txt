cmake_minimum_required(VERSION 3.27)
project(regex)

set(CMAKE_CXX_STANDARD 23)

add_library(dfa_minimizer STATIC src/dfa_minimizer.cpp)
target_include_directories(dfa_minimizer PUBLIC include)
target_link_libraries(dfa_minimizer dfa)