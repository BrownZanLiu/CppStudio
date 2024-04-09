cmake_minimum_required(VERSION 3.1...3.26)

file(
	GLOB
	DSAAME_LINKEDLISTS_SRC
	"RecursionAndBacktrack/*.cpp"
	"RecursionAndBacktrack/*.h"
	"LinkedLists/*.cpp"
	"LinkedLists/*.cxx"
	"LinkedLists/*.h"
	"LinkedLists/*.hpp"
)
add_executable(
	dsaame
	${DSAAME_LINKEDLISTS_SRC}
)
target_compile_features(
	dsaame
	PRIVATE
	cxx_std_20
)
if (${COMPILER_SUITE} STREQUAL "gcc")
  	target_link_options(
  		dsaame
  		PRIVATE
  		-static-libgcc
  		-static-libstdc++
  	)
	if (${MY_GCC_VERSION} VERSION_LESS "10.0.0")
		target_link_libraries(
			dsaame
			# Gcc support for code generation of C++ error handling, RTTI, etc
			/usr/local/gcc-${MY_GCC_VERSION}/lib/gcc/x86_64-pc-linux-gnu/${MY_GCC_VERSION}/libgcc_eh.a
			# No shared version, so maybe ok to omitted
			/usr/local/gcc-${MY_GCC_VERSION}/lib64/libstdc++fs.a
		)
	endif()
endif()
target_link_libraries(
	dsaame
	gtest_main
	gmock_main
)

