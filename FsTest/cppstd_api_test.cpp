// C++ standard libraries
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

// third party libraries
#include <gtest/gtest.h>

// local headers
#include "fstest_flags.h"

TEST(CppStdApiTest, CreateFile)
{
	namespace fs = std::filesystem;

	fs::path vFileName{FLAGS_pathname};
	std::cout << "Try to create a file: " << vFileName << std::endl;
	std::fstream vFile;
	vFile.open(vFileName, std::ios::out | std::ios::noreplace);
	if (!vFile.is_open()) {
		std::cout << "Try to open an existed file: " << vFileName << std::endl;
		return;
	}
	vFile << "Hi,Brown! This is a test of std::fstream::open()." << std::endl;
	std::cout << "Done with creation of file: " << vFileName << std::endl;
}

