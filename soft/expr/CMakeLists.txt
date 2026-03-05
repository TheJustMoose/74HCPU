cmake_minimum_required(VERSION 4.2)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "../../Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "../../Release")

project(simple_expr CXX)
add_executable(simple_expr
	lexer.h
	plus.cpp
)

include_directories("../doctest")

project(unit_tests CXX)
add_executable(simple_expr_unit_tests
	lexer.cc
	plus.cpp
)
