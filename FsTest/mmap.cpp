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
#include "libfstest.h"


TEST(TSMemMap, TCLoadFile)
{
	using namespace liuzan::fstest;

	const int PAGES_TO_WRITE = PAGES_PER_GIGA;

	try {
		std::string vTestDir = FLAGS_root + "/mmap";

		std::cout << NowString() << "Try to make directory: " << vTestDir << std::endl;
		MkDir(vTestDir, S_IRWXU);

		std::string vSourceFile = vTestDir + "/file4mmap.bin";
		std::cout << NowString() << "Try to create file: " << vSourceFile << std::endl;
		int vFd = CreateFile(vSourceFile, O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);

		std::cout << NowString()
			      << "Try to writing " << PAGES_TO_WRITE << " pages into file: " << vSourceFile << std::endl;
		for (uint32_t pi = 0; pi < PAGES_TO_WRITE; ++pi) {
			WritePage(vFd, pi);
		}

		std::cout << NowString() << "Try to close and reopen file: " << vSourceFile << std::endl;
		CloseFd(vFd);
		vFd = OpenPath(vSourceFile, O_RDONLY);

		std::cout << NowString() << "Try to stat fd: " << vFd << std::endl;
		struct stat vStat;
		StatFd(vFd, &vStat);

		std::cout << NowString() << "Try to mmap fd: " << vFd << std::endl;
		MemMapArg vMapArg(vFd, MAP_SHARED | MAP_LOCKED, PROT_READ, vStat.st_size);
		//MemMapArg vMapArg(vFd, MAP_SHARED | MAP_POPULATE, PROT_READ, vStat.st_size);
		long *vCpuWord = static_cast<long *>(MemMap(vMapArg));
		CloseFd(vFd);

		std::cout << NowString() << "Try to read with mmap: " << vCpuWord << std::endl;
		long vCnt = vStat.st_size / sizeof(long);
		long vSum = 0;
		for (long i = 0; i < vCnt; ++i) {
			vSum += vCpuWord[i];
		}

		std::cout << NowString() << "Try to munmap: " << vCpuWord << std::endl;
		MemUnmapArg vUnmapArg(static_cast<void *>(vCpuWord), vStat.st_size);
		MemUnmap(vUnmapArg);

		std::cout << NowString() << "Work done! Sum: " << vSum << std::endl;
	} catch (const std::string & e) {
		std::cout << e << std::endl;
		EXPECT_FALSE(true);
		return;
	}
}


