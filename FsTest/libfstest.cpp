#include <chrono>  // std::time_point
#include <cstdint>  // uint64_t
#include <iostream>
#include <string>

#include <file_system.h>

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

