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

#### Toolchain configurations ####
set(COMPILER_SUITE "gcc"
	CACHE PATH ""
	FORCE
)
if (${COMPILER_SUITE} STREQUAL "gcc")
	set(MY_GCC_VERSION "12.2.0")
	set(CMAKE_C_COMPILER "/usr/local/gcc-${MY_GCC_VERSION}/bin/gcc-${MY_GCC_VERSION}"
		CACHE PATH ""
		FORCE
	)
	set(CMAKE_CXX_COMPILER "/usr/local/gcc-${MY_GCC_VERSION}/bin/g++-${MY_GCC_VERSION}"
		CACHE PATH ""
		FORCE
	)
else (${COMPILER_SUITE} STREQUAL "clang")
	set(CMAKE_C_COMPILER "/usr/bin/clang"
		CACHE PATH ""
		FORCE
	)
	set(CMAKE_CXX_COMPILER "/usr/bin/clang++"
		CACHE PATH ""
		FORCE
	)
endif()

#### Add github helper module ####
# https://github.com/CLIUtils/cmake
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

#### Fetch external package ####
include(FetchContent)
FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG 58d77fa8070e8cec2dc1ed015d66b454c8d78850  # release-1.12.1
)
FetchContent_Declare(
	gflags
	GIT_REPOSITORY https://github.com/gflags/gflags.git
	GIT_TAG e171aa2d15ed9eb17054558e0b3a6a413bb01067 # v2.2.2
)
#FetchContent_Declare(
#	grpc
#	GIT_REPOSITORY https://github.com/grpc/grpc.git
#	GIT_TAG 8871dab19b4ab5389e28474d25cfeea61283265c # v1.54.2
#)
FetchContent_MakeAvailable(googletest gflags)  # grpc)

#### Making an demo executable ####
add_executable(
	demo  # name of application/executable; also name of cmake target
	demo.cpp  # list of source files
	demo.h  # list of header files just for IDE display
)
target_compile_features(
	demo
	PRIVATE
	cxx_std_23
)
if (${COMPILER_SUITE} STREQUAL "gcc")
	target_link_options(
		demo
		PRIVATE
		-static-libgcc  # integer/floating-point arithmetics and so on used by compiler
		-static-libstdc++
	)
	target_link_libraries(
		demo
		gcc_eh  # Gcc support for code generation of C++ error handling, RTTI, etc
		stdc++fs  # No shared version, so maybe ok to omitted
	)
endif()

#### Add subdirectories ####
add_subdirectory(CppUsage)

