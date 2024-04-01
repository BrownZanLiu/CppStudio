#include <chrono>  // std::time_point
#include <cstdint>  // uint64_t
#include <iostream>
#include <future>  // std::async, std::future, etc
#include <sstream>  // std::ostringstream
#include <string>

#include <gflags/gflags.h>
#include <gtest/gtest.h>

#include <file_system.h>

#include "fstest_flags.h"

namespace liuzan {
namespace fstest {

using SystemClock = std::chrono::system_clock;
using SystemTime = std::chrono::time_point<SystemClock>;
using namespace liuzan::filesystem;

SystemTime Now()
{
	return SystemClock::now();
}

std::string NowString()
{
	auto vNow = Now();
	std::ostringstream vNowString;
	vNowString << "[" << vNow << "]";
	return vNowString.str();
}

void * GetPageData(uint32_t pageNum)
{
	static uint64_t vPageContent[QWORD_PER_PAGE];

	for (uint32_t qi = 0; qi < QWORD_PER_PAGE; ++qi) {
		vPageContent[qi] = static_cast<uint64_t>(pageNum + 13) << 32 | static_cast<uint64_t>(qi);
	}

	return static_cast<void *>(vPageContent);
}

void WritePage(int fd, uint32_t pageIndex)
{
	ssize_t vRc = write(fd, GetPageData(pageIndex), BYTES_PER_PAGE);

	if (vRc == -1)	{
		throw std::string("FileSystem API exception triggered by WritePage: ") + strerror(errno);
	} else if (vRc != BYTES_PER_PAGE) {
		throw std::string("FileSystem API exception triggered by WritePage: ") + std::to_string(vRc);
	}
}

}  // namespace fstest
}  // namespace liuzan

TEST(TSSendfile, TCSingleShot)
{
	using namespace liuzan::fstest;

	const int PAGES_TO_WRITE = PAGES_PER_GIGA;

	try {
		std::string vTestDir = FLAGS_root + "/sendfile";

		std::cout << NowString() << "Try to make directory: " << vTestDir << std::endl;
		MakeDir(vTestDir, S_IRWXU);

		std::string vSourceFile = vTestDir + "/source.bin";
		std::cout << NowString() << "Try to create file: " << vSourceFile << std::endl;
		int vSrcFd = CreateFile(vSourceFile, O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);

		std::cout << NowString()
			      << "Try to writing " << PAGES_TO_WRITE << " pages into file: " << vSourceFile << std::endl;
		for (uint32_t pi = 0; pi < PAGES_TO_WRITE; ++pi) {
			WritePage(vSrcFd, pi);
		}

		std::cout << NowString() << "Try to close and reopen file: " << vSourceFile << std::endl;
		CloseFd(vSrcFd);
		vSrcFd = OpenPath(vSourceFile, O_RDONLY);

		std::cout << NowString() << "Try to stat fd: " << vSrcFd << std::endl;
		struct stat vSrcStat;
		StatFd(vSrcFd, &vSrcStat);

		std::string vDestFile = vTestDir + "/dest.bin";
		std::cout << NowString() << "Try to create file: " << vDestFile << std::endl;
		int vDestFd = CreateFile(vDestFile, O_TRUNC | O_WRONLY, vSrcStat.st_mode);

		std::cout << NowString() << "Try to send " << vSourceFile << " to " << vDestFile << std::endl;
		SendFile(vDestFd, vSrcFd, nullptr, vSrcStat.st_size);

		std::cout << NowString() << "Try to close fd: " << vDestFd << std::endl;
		CloseFd(vDestFd);
		std::cout << NowString() << "Try to close fd: " << vSrcFd << std::endl;
		CloseFd(vSrcFd);

		std::cout << NowString() << "work done!" << std::endl;
	} catch (const std::string & e) {
		std::cout << e << std::endl;
		EXPECT_FALSE(true);
		return;
	}
}

TEST(TSSendfile, TCReadWrite4PerfCompare)
{
	using namespace liuzan::fstest;

	const int PAGES_TO_WRITE = PAGES_PER_GIGA;
	char page_bytes[BYTES_PER_PAGE];
	for (int i = 0; i < BYTES_PER_PAGE; ++i) {
		page_bytes[i] = 'x';
	}

	try {
		std::string vTestDir = FLAGS_root + "/read_write";

		std::cout << NowString() << "Try to make directory: " << vTestDir << std::endl;
		MakeDir(vTestDir, S_IRWXU);

		std::string vSourceFile = vTestDir + "/source.bin";
		std::cout << NowString() << "Try to create file: " << vSourceFile << std::endl;
		int vSrcFd = CreateFile(vSourceFile, O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);

		std::cout << NowString()
			      << "Try to writing " << PAGES_TO_WRITE << " pages into file: " << vSourceFile << std::endl;
		for (uint32_t pi = 0; pi < PAGES_TO_WRITE; ++pi) {
			WritePage(vSrcFd, pi);
		}

		std::cout << NowString() << "Try to close and reopen file: " << vSourceFile << std::endl;
		CloseFd(vSrcFd);
		vSrcFd = OpenPath(vSourceFile, O_RDONLY);

		std::cout << NowString() << "Try to stat fd: " << vSrcFd << std::endl;
		struct stat vSrcStat;
		StatFd(vSrcFd, &vSrcStat);

		std::string vDestFile = vTestDir + "/dest.bin";
		std::cout << NowString() << "Try to create file: " << vDestFile << std::endl;
		int vDestFd = CreateFile(vDestFile, O_TRUNC | O_WRONLY, vSrcStat.st_mode);

		std::cout << NowString() << "Try to copy " << vSourceFile << " to " << vDestFile << std::endl;
		for (uint32_t pi = 0; pi < PAGES_TO_WRITE; ++pi) {
			ReadFile(vSrcFd, page_bytes, BYTES_PER_PAGE);
			WriteFile(vDestFd, page_bytes, BYTES_PER_PAGE);
		}

		std::cout << NowString() << "Try to close fd: " << vDestFd << std::endl;
		CloseFd(vDestFd);
		std::cout << NowString() << "Try to close fd: " << vSrcFd << std::endl;
		CloseFd(vSrcFd);

		std::cout << NowString() << "work done!" << std::endl;
	} catch (const std::string & e) {
		std::cout << e << std::endl;
		EXPECT_FALSE(true);
		return;
	}
}

