cmake_minimum_required(VERSION 4.2)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "../Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "../Release")

project(asm CXX)
add_executable(uemul
	commands.cc
	cpu.cc
	offset2int.cc
	read_hex.cc
	uemul.cc
)

include_directories("doctest/doctest")

project(unit_tests CXX)
add_executable(unit_tests
	commands.cc
	cpu.cc
	offset2int.cc
	unit_tests.cc
)
