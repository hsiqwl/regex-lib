cmake_minimum_required(VERSION 3.27)
project(state)

set(CMAKE_CXX_STANDARD 23)

add_library(state STATIC src/state.cpp)
add_compile_options(-g -fsanitize=address)
add_link_options(-fsanitize=address)
target_include_directories(state PUBLIC include)