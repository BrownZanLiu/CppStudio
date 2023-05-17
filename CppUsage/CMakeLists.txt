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
		# The following two libraries are only supported ones with -static-* link options.
		# It's also OK to do it with absolute libraries in target_link_libraries
		# GCC support for code generation of integer/floating-point arithmetics, etc
  		-static-libgcc
  		-static-libstdc++
  	)
	target_link_libraries(
		cppusage
		# The following could be done with target_link_options::-static-libgcc
		#/usr/local/gcc-${MY_GCC_VERSION}/lib/gcc/x86_64-pc-linux-gnu/${MY_GCC_VERSION}/libgcc.a
		# The following could be done with target_link_options::-static-libstdc++
		#/usr/local/gcc-${MY_GCC_VERSION}/lib64/libstdc++.a

		# Gcc support for code generation of C++ error handling, RTTI, etc
		/usr/local/gcc-${MY_GCC_VERSION}/lib/gcc/x86_64-pc-linux-gnu/${MY_GCC_VERSION}/libgcc_eh.a
		# No shared version, so maybe ok to omitted
		/usr/local/gcc-${MY_GCC_VERSION}/lib64/libstdc++fs.a
	)
endif()
target_link_libraries(
	cppusage
	gtest_main
	gmock_main
	gflags_nothreads_static
	#grpc
)

