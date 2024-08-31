cmake_minimum_required(VERSION 3.27)
project(ast)

set(CMAKE_CXX_STANDARD 23)

add_library(ast STATIC src/node.cpp src/ast.cpp)
add_compile_options(-g -fsanitize=address)
add_link_options(-fsanitize=address)
target_include_directories(ast PUBLIC include)
target_link_libraries(ast token)