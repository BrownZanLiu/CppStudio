cmake_minimum_required(VERSION 3.1...3.26)

file(
	GLOB
	LEETCODE_MICROSOFT_SRC
	"./Microsoft/*.cpp"
	"./Microsoft/*.cxx"
	"./Microsoft/*.h"
	"./Microsoft/*.hpp"
)

add_executable(
	run_leetcode
	${LEETCODE_MICROSOFT_SRC}
)
target_include_directories(
	run_leetcode
	PRIVATE
	../Common/Include
)
target_compile_features(
	run_leetcode
	PRIVATE
	cxx_std_20
)
if (${COMPILER_SUITE} STREQUAL "gcc")
  	target_link_options(
  		run_leetcode
  		PRIVATE
  		-static-libgcc
  		-static-libstdc++
  	)
	if (${MY_GCC_VERSION} VERSION_LESS "10.0.0")
		target_link_libraries(
			run_leetcode
			# Gcc support for code generation of C++ error handling, RTTI, etc
			/usr/local/gcc-${MY_GCC_VERSION}/lib/gcc/x86_64-pc-linux-gnu/${MY_GCC_VERSION}/libgcc_eh.a
			# No shared version, so maybe ok to omitted
			/usr/local/gcc-${MY_GCC_VERSION}/lib64/libstdc++fs.a
		)
	endif()
endif()
target_link_libraries(
	run_leetcode
	gtest_main
)

