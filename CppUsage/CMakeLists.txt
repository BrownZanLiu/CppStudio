cmake_minimum_required(VERSION 3.1...3.26)

file(GLOB CPPUSAGE_SRC "*.cc" "*.cpp" "*.cxx" "*.h" "*.hpp")
add_executable(
	cppusage
	${CPPUSAGE_SRC}
)
target_compile_features(
	cppusage
	PRIVATE
	cxx_std_20
)
if (${COMPILER_SUITE} STREQUAL "gcc")
  	target_link_options(
  		cppusage
  		PRIVATE
  		-static-libgcc
  		-static-libstdc++
  	)
	if (${MY_GCC_VERSION} VERSION_LESS "10.0.0")
		target_link_libraries(
			cppusage
			# Gcc support for code generation of C++ error handling, RTTI, etc
			/usr/local/gcc-${MY_GCC_VERSION}/lib/gcc/x86_64-pc-linux-gnu/${MY_GCC_VERSION}/libgcc_eh.a
			# No shared version, so maybe ok to omitted
			/usr/local/gcc-${MY_GCC_VERSION}/lib64/libstdc++fs.a
		)
	endif()
endif()
target_link_libraries(
	cppusage
	gtest_main
	gmock_main
	gflags_nothreads_static
	#grpc
)

