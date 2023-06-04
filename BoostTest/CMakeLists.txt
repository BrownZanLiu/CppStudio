cmake_minimum_required(VERSION 3.20...3.26)
file(GLOB BOOST_TESTS_SRC "*.cpp" "*.cxx" "*.h" "*.hpp")
add_executable(
	boost_tests
	${BOOST_TESTS_SRC}
)
target_compile_features(
	boost_tests
	PRIVATE
	cxx_std_23
)
if (${COMPILER_SUITE} STREQUAL "gcc")
  	target_link_options(
		boost_tests
  		PRIVATE
  		-static-libgcc
  		-static-libstdc++
  	)
	if (${MY_GCC_VERSION} VERSION_LESS "10.0.0")
		target_link_libraries(
			boost_tests
			# Gcc support for code generation of C++ error handling, RTTI, etc
			/usr/local/gcc-${MY_GCC_VERSION}/lib/gcc/x86_64-pc-linux-gnu/${MY_GCC_VERSION}/libgcc_eh.a
			# No shared version, so maybe ok to omitted
			/usr/local/gcc-${MY_GCC_VERSION}/lib64/libstdc++fs.a
		)
	endif()
endif()
file(GLOB BOOST_INCLUDE_ROOT_DIRS ../boost/libs/*/include ../boost/libs/*/*/include)
target_include_directories(
	boost_tests
	SYSTEM
	PRIVATE
	${BOOST_INCLUDE_ROOT_DIRS}
)

