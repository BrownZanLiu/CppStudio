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

#### Add github helper module ####
# https://github.com/CLIUtils/cmake
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

#### Global configurations ####
#set(MY_GCC_VERSION "12.2.0")
#set(CMAKE_C_COMPILER "/usr/local/gcc-${MY_GCC_VERSION}/bin/gcc-${MY_GCC_VERSION}")
#set(CMAKE_CXX_COMPILER "/usr/local/gcc-${MY_GCC_VERSION}/bin/g++-${MY_GCC_VERSION}")
#set(
#	PATH_TO_STATIC_CXX_STD_LIBRARY
#	"/usr/local/gcc-${MY_GCC_VERSION}/lib64/libstdc++.a"
#	CACHE
#	PATH
#	""
#)

#### Making an executable ####
add_executable(
	demo  # name of application/executable; also name of cmake target
	demo.cpp  # list of source files
	demo.h  # list of header files just for IDE display
)
target_compile_features(
	demo
	PRIVATE
	cxx_std_20
)
#target_link_options(
#	demo
#	PRIVATE
#	-static-libgcc
#	-static-libstdc++
#)
#target_link_libraries(
#	demo
#	PRIVATE
#	"${PATH_TO_STATIC_CXX_STD_LIBRARY}"
#)

#### Add subdirectories ####
add_subdirectory(CppUsage)

####################################################################################################
# Followings are just for references.
####################################################################################################

#### Making a library ####
#add_library(
#	LibName
#	[STATIC]  # Optional: STATIC, SHARED or MODULE; defined by BUILD_SHARED_LIBS if ommited
#	[libsrc.cpp] # Optional for fictional library; also called INTERFACE library
#	libheader.h
#)

#### Target settings ####
# Specify header searching path
#target_include_directories(
#	TargetName  # defined by add_executable() or add_library(); must not be ALAS target
#	[SYSTEM]
#	[AFTER|BEFORE]
#	<INTERFACE|PUBLIC|PRIVATE>
#	# Meaningless for executables.
#	# PUBLIC choosen, any target linking to the target library must also need following directory.
#	# PRIVATE choosen, only affect current target with no dependencies propagation.
#	# INTERFACE choosen, only needed for dependencies.
#	# PUBLIC | PRIVATE updates INCLUDE_DIRECTORES
#	# INTERFACE | PUBLIC updates INTERFACE_INCLUDE_DIRECTORIES
#   IncludeDirectories
#	[INTERFACE|PUBLIC|PRIVATE]
#   [OtherIncludeDirectories]
#   ...
#)
# Specify denpendences on other targets or links to another libraries
#target_link_libraries(
#	TargetNameHasDependenciesOnFollowings
#	<INTERFACE|PUBLIC|PRIVATE>
#	TargetsBeDependedOn
#	[<INTERFACE|PUBLIC|PRIVATE>]
#	[OtherTargetsBeDependedOn]
#	...
#)
#target_link_options()
# Specify languages features needed.
#target_compile_features(
#	TargetName
#	PUBLIC
#	cxx_std_11
#)
#target_compile_options()
#target_compile_definitions()
#target_precompile_headers()
#target_sources()

#### Local Variables ####
#set(MY_VARIABLE "value")  # Refers it as ${MY_VARIABLE}. For path, better with "${MY_VARAIBLE}"
#set(MY_LIST "one" "two)  # Identical to set(MY_LIST "one;two")
#set(MY_CACHE_VARIABLE "test" CACHE STRING "Description: This is just a test.")
#set(MY_CACHE_VARIABLE "brown" CACHE STRING "Description: could be empty, but must here")
#set(MY_CACHE_VARIABLE "liuzan" CACHE STRING "")
#set(MY_CACHE_VARIABLE "c++23" CACHE STRING "" FORCE)
#mark_as_advanced(MY_CACHE_VARIABLE)
#set(MY_CACHE_VARIABLE "c++26" CACHE INTERNAL "")  # Same as mark_as_advanced to avoid -L print
#option(MY_OPTION "This is settable from the command line" OFF)  # OFF|NO|FALSE|N, ON|YES|TRUE|Y

#### Properties ####
#set_property(
#	TARGET TargetName
#	PROPERTY CXX_STANDARD 11
#)
#set_target_properties(
#	TargetName
#	PROPERTIES
#	CXX_STANDARD 11
#	[OTHER_PROPERTY val]
#	...
#)
#get_property(
#	ResultVaraible
#	TARGET TargetName
#	PROPERTY CXX_STANDARD
#)

#### Control Flow ####
#Unary keywords: NOT, TARGET, EXISTS(file), DEFINED, etc.
#Binary keywords: STREQUAL, AND, OR, MATCHES(regular expression), VERSION_LESS, VERSION_LESS_EQUAL, etc
#Parentehses can be used to group
#if(${MY_CONDITION})
#	#If 'ON', 'YES', 'TRUE', 'Y' or non-zero number, do something.
#else()
#	#If 'OFF', 'NO', 'FALSE', 'N', '0', 'IGNORE', 'NOTFOUND', '""' or ends in '-NOTFOUND', do sth.
#endif()
##If not expand to one of the above, expand and try again above.
#if("${MY_CONDITION}")  # After quoted, no try again.
#	#If not false-like, do something.
#else()
#	#If 'OFF', 'NO', 'FALSE', 'N', '0', 'IGNORE', 'NOTFOUND', '""' or ends in '-NOTFOUND', do sth.
#endif()

#### Macroes and Functions ####
#function(FUNC_TEST)
#	message(STATUS "Function test: ${ARGN}")
#endfunction()
#func_test(This is a test of function)

