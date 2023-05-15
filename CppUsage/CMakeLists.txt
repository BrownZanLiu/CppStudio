cmake_minimum_required(VERSION 3.20...3.26)

file(GLOB CPPUSAGE_SRC "*.cpp" "*.cxx" "*.h" "*.hpp")
add_executable(
	cppusage
	${CPPUSAGE_SRC}
)

#target_include_directories(
#	cppusage
#	BEFORE
#	PRIVATE
#	/usr/local/gcc-${MY_GCC_VERSION}/include/c++/${MY_GCC_VERSION}
#)

#target_link_libraries(
#	cppusage
#	PRIVATE
#	/usr/local/gcc-${MY_GCC_VERSION}/lib64/libstdc++.a
#	/usr/local/gcc-${MY_GCC_VERSION}/lib64/libstdc++fs.a
#)

