cmake_minimum_required(VERSION 3.20...3.26)

file(
	GLOB FSTEST_SRC
	"*.cc"
	"*.cpp"
	"*.cxx"
	"*.h"
	"*.hpp"
)
file(
	GLOB LIBPOSIXFS_SRC
	"../libposixfs/*.cc"
	"../libposixfs/*.cpp"
	"../libposixfs/*.cxx"
	"../libposixfs/*.h"
	"../libposixfs/*.hpp"
)
add_executable(
	fstest
	${FSTEST_SRC}
	${LIBPOSIXFS_SRC}
)

target_include_directories (
	fstest
	PRIVATE
	"../include"
)

target_compile_features(
	fstest
	PRIVATE
	cxx_std_23
)

if (${COMPILER_SUITE} STREQUAL "gcc")
  	target_link_options(
  		fstest
  		PRIVATE
  		-static-libgcc
  		-static-libstdc++
  	)
	if (${MY_GCC_VERSION} VERSION_LESS "10.0.0")
		target_link_libraries(
			fstest
			# Gcc support for code generation of C++ error handling, RTTI, etc
			/usr/local/gcc-${MY_GCC_VERSION}/lib/gcc/x86_64-pc-linux-gnu/${MY_GCC_VERSION}/libgcc_eh.a
			# No shared version, so maybe ok to omitted
			/usr/local/gcc-${MY_GCC_VERSION}/lib64/libstdc++fs.a
		)
	endif()
endif()

target_link_libraries(
	fstest
	gtest
	gmock
	gflags_nothreads_static
)

