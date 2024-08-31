cmake_minimum_required(VERSION 3.27)
project(trie)

set(CMAKE_CXX_STANDARD 23)

add_library(trie STATIC src/trie.cpp)
target_include_directories(trie PUBLIC include)
