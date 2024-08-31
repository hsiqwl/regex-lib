cmake_minimum_required(VERSION 3.27)
project(dfa)

set(CMAKE_CXX_STANDARD 23)
add_library(dfa STATIC src/dfa.cpp)
add_compile_options(-g -fsanitize=address)
add_link_options(-fsanitize=address)
target_include_directories(dfa PUBLIC include)
target_link_libraries(dfa state)