#### MUST: Minimum version requirement for C++ ####
# Minimum for C++20: cmake 3.12
# Minimum for C++23: cmake 3.20
# Minimum for C++26: cmake 3.25
#### MUST: Minimum version requirement for C++ ####
cmake_minimum_required(VERSION 3.20...3.26)

#### MUST: setting project in top-level CMake file ####
project(
	CppStudio  # project name
	VERSION 1.0.0
	DESCRIPTION "All kinds of test around C++ core languages or C++ libraries."
	LANGUAGES CXX
)

#### Making an executable ####
add_executable(
	demo  # name of application/executable; also name of cmake target
	demo.cpp  # list of source files
	demo.h  # list of header files just for IDE display
)
