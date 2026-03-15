cmake_minimum_required(VERSION 4.2)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "../../Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "../../Release")

project(simple_expr CXX)
add_executable(simple_expr
	assemble.h
	assemble.cc
	func_guard.h
	func_guard.cc
	lexer.h
	lexer.cc
	main.cc
	names.cc
	node.h
	node_type.h
	optimize.h
	optimize.cc
	parser.h
	parser.cc
	print_tree.h
	print_tree.cc
	token.h
	var.cc
	var.h
	var_size.cc
	var_size.h
)

include_directories("../doctest/doctest")

project(unit_tests CXX)
add_executable(simple_expr_unit_tests
	assemble.h
	assemble.cc
	func_guard.h
	func_guard.cc
	lexer.h
	lexer.cc
	names.cc
	node.h
	node_type.h
	optimize.h
	optimize.cc
	parser.h
	parser.cc
	print_tree.h
	print_tree.cc
	simple_expr_unit_tests.cc
	token.h
	var.cc
	var.h
	var_size.cc
	var_size.h
)
