// C++ standard libraries
#include <iostream>
#include <filesystem>
#include <string>

// system headers
#include <fcntl.h>  // Definition of O_* and S_* constants
#include <linux/openat2.h>  // Definition of RESOLVE_* constants
#include <sys/syscall.h>  // Definition of SYS_* constants
#include <unistd.h>

// third party libraries
#include <gtest/gtest.h>

// local headers
#include "fstest_flags.h"

TEST(GlibcApiTest, CreateFile)
{
	std::cout << "To be defined!" << std::endl;
}

