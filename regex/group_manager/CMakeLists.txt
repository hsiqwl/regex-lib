cmake_minimum_required(VERSION 3.27)
project(group_tracker)

set(CMAKE_CXX_STANDARD 23)

add_library(group_tracker STATIC src/group_tracker.cpp src/group_manager.cpp)
target_include_directories(group_tracker PUBLIC include)
target_link_libraries(group_tracker nfa_simulator)