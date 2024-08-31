cmake_minimum_required(VERSION 3.27)
project(match_results)

set(CMAKE_CXX_STANDARD 23)

add_library(match_results STATIC src/match_results.cpp)
target_include_directories(match_results PUBLIC include)
target_link_libraries(match_results regex_engine)