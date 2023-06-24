cmake_minimum_required(VERSION 3.20...3.26)

file(GLOB COMMON_SRC_SRC "./Src/*.cpp" "./Src/*.cxx" "./Src/*.h" "./Src/*.hpp")
file(GLOB COMMON_TEST_SRC "./Test/*.cpp" "./Test/*.cxx" "./Test/*.h" "./Test/*.hpp")

add_executable(
	test_common
	${COMMON_SRC_SRC}
	${COMMON_TEST_SRC}
)
target_include_directories(
	test_common
	PRIVATE
	./Include
)
target_compile_features(
	test_common
	PRIVATE
	cxx_std_23
)
if (${COMPILER_SUITE} STREQUAL "gcc")
  	target_link_options(
  		test_common
  		PRIVATE
  		-static-libgcc
  		-static-libstdc++
  	)
	if (${MY_GCC_VERSION} VERSION_LESS "10.0.0")
		target_link_libraries(
			test_common
			# Gcc support for code generation of C++ error handling, RTTI, etc
			/usr/local/gcc-${MY_GCC_VERSION}/lib/gcc/x86_64-pc-linux-gnu/${MY_GCC_VERSION}/libgcc_eh.a
			# No shared version, so maybe ok to omitted
			/usr/local/gcc-${MY_GCC_VERSION}/lib64/libstdc++fs.a
		)
	endif()
endif()
target_link_libraries(
	test_common
	gtest_main
	#gmock_main
)

