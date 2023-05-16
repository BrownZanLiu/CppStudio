cmake_minimum_required(VERSION 3.20...3.26)

file(GLOB CPPUSAGE_SRC "*.cpp" "*.cxx" "*.h" "*.hpp")
add_executable(
	cppusage
	${CPPUSAGE_SRC}
)
target_compile_features(
	cppusage
	PRIVATE
	cxx_std_23
)
if (${COMPILER_SUITE} STREQUAL "gcc")
	target_link_options(
		cppusage
		PRIVATE
		-static-libgcc  # GCC support for code generation of integer/floating-point arithmetics, etc
		-static-libstdc++
	)
	target_link_libraries(
		cppusage
		gcc_eh  # Gcc support for code generation of C++ error handling, RTTI, etc
		stdc++fs  # No shared version, so maybe ok to omitted
	)
endif()
target_link_libraries(
	cppusage
	gtest_main
	gmock_main
	gflags_nothreads_static
	#grpc  # Don't work for now
)

