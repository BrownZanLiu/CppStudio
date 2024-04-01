// C++ standard libraries
#include <cstdint>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <system_error>
#include <string>

// third party libraries
#include <gtest/gtest.h>

// local headers
#include "fstest_flags.h"

/** ==== global alias ==== **/
namespace stdfs = std::filesystem;
using stdios = std::ios;

TEST(CppStdApiTest, CreateFile)
{
	stdfs::path vFileName{FLAGS_pathname};
	vFileName = stdfs::absolute(vFileName);
	std::cout << "Try to create a file: " << vFileName << std::endl;
	std::fstream vFile;
	vFile.open(vFileName, stdios::out | stdios::noreplace);
	if (!vFile.is_open()) {
		std::cout << "Failed to create an file: " << vFileName << std::endl;
		return;
	}
	vFile << "Hi,Brown! This is a test of std::fstream::open()." << std::endl;
	std::cout << "Done with creation of file: " << vFileName << std::endl;
}

TEST(CppStdApiTest, DeleteFile)
{
	stdfs::path vFileName{FLAGS_pathname};
	vFileName = stdfs::absolute(vFileName);
	std::error_code vEc;
	bool vSucceded;
	vSucceded = stdfs::remove(vFileName, vEc);
	if (vSucceded) {
		std::cout << "Succeded to remove file: " << vFileName << std::endl;
	} else if (vEc.value() == 0) {
		std::cout << "Try to remove a non-existed file: " << vFileName << std::endl;
	} else {
		std::cout << "Failed to remove file: " << vFileName << std::endl;
		std::cout << "Error info: " << vEc.message() << std::endl;
	}

}

void CreateFilesUnderGivenDir(stdfs::path const & inParentDir, uint64_t inFileNum)
{
	std::ofstream vFileStream;
	for(uint64_t vFileCnt = 0llu; vFileCnt < inFileNum; ++vFileCnt) {
		stdfs::path vFilePath{inParentDir.string() + "_" + std::to_string(vFileCnt)};
		vFileStream.open(vFilePath, stdios::out | stdios::noreplace);
		if (!vFileStream.is_open()) {
			EXPECT_FALSE(true) << "Failed to open " << vFilePath;
			continue;
		}
		vFileStream << vFilePath;
		vFileStream.close();
	}
}

TEST(CppStdApiTest, CreateFiles)
{
}
