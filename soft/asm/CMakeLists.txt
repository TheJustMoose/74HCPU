cmake_minimum_required(VERSION 4.2)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "../../Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "../../Release")

project(asm CXX)
add_executable(74hc-asm
	assm.cc
	error_collector.cc
	file_reader.cc
	main.cc
	preprocessor.cc
	str_util.cc
)

include_directories("doctest/doctest")

project(unit_tests CXX)
add_executable(asm_unit_tests
	assm.cc
	error_collector.cc
	file_reader.cc
	preprocessor.cc
	str_util.cc
	unit_tests_cop.cc
	unit_tests_names.cc
	unit_tests_preprocessor.cc
	unit_tests_util.cc
)
