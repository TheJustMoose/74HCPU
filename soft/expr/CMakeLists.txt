cmake_minimum_required(VERSION 4.2)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "../../Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "../../Release")

project(simple_expr CXX)
add_executable(simple_expr
	assemble.h
	assemble.cc
	data_type.h
	data_type.cc
	func_guard.h
	func_guard.cc
	lexer.h
	lexer.cc
	main.cc
	names.h
	names.cc
	node.h
	node_type.h
	nodes.h
	nodes.cc
	optimize.h
	optimize.cc
	parser.h
	parser.cc
	print_tree.h
	print_tree.cc
	token.h
	token2node_type.h
	token2node_type.cc
	var.h
	var.cc
	var_size.h
	var_size.cc
)

include_directories("../doctest/doctest")

project(unit_tests CXX)
add_executable(simple_expr_unit_tests
	assemble.h
	assemble.cc
	data_type.h
	data_type.cc
	func_guard.h
	func_guard.cc
	lexer.h
	lexer.cc
	names.h
	names.cc
	node.h
	node_type.h
	nodes.h
	nodes.cc
	optimize.h
	optimize.cc
	parser.h
	parser.cc
	print_tree.h
	print_tree.cc
	simple_expr_unit_tests.cc
	token.h
	token2node_type.h
	token2node_type.cc
	var.h
	var.cc
	var_size.h
	var_size.cc
)
