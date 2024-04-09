set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86)

set(MY_GCC_VERSION 11.4.0)
set(COMPILER_BIN /usr/local/gcc-${MY_GCC_VERSION}/bin)
set(CMAKE_C_COMPILER ${COMPILER_BIN}/gcc-${MY_GCC_VERSION})
set(CMAKE_CXX_COMPILER ${COMPILER_BIN}/g++-${MY_GCC_VERSION})
