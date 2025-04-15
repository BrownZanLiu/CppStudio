#include <iostream>
#include <future>  // std::async, std::future, etc
#include <type_traits>  // std::invoke_result_t<>
#include <vector>

#include <gflags/gflags.h>
#include <gtest/gtest.h>

#include <file_system.h>
#include <host_info.h>  // liuzan::HostInfo

#include "fstest_flags.h"
#include "libfstest.h"

using namespace std::literals;

using namespace liuzan;
using namespace liuzan::fstest;
using namespace liuzan::filesystem;

TEST(TSFile, TCCreateFileOneShot)
{
	std::string vFile2Create = GetTestRootDir() + "/file.bin";
	CreateFileArg vCreateArg{vFile2Create};
	std::cout << NowString() << "Try to create " << vFile2Create << std::endl;
	vCreateArg.fsIoStat->Clear(FsOpId::CREATE);

	try {
		EXPECT_TRUE(vCreateArg.fsIoStat->StartOrReStartAccounting());
		int vFd = CreateFile(vCreateArg);
		EXPECT_TRUE(vCreateArg.fsIoStat->StopIfAccounting());
		for (int i = 0; i < FLAGS_filesize / BYTES_PER_PAGE; ++i) {
			WritePage(vFd, i);
		}
		CloseFd(vFd);

		std::cout << NowString() << "Succeeded to create file in "
			<< vCreateArg.fsIoStat->AccumulatedMeasure(FsOpId::CREATE)
			<< " us." << std::endl;
	} catch (std::system_error &e) {
		std::cout << e.what() << std::endl;
		EXPECT_FALSE(true);
	}
}

TEST(TSFile, TCDeleteFileOneShot)
{
	std::string vFile2Del = GetTestRootDir() + "/file.bin";
	UnlinkPathArg vDelArg{vFile2Del};
	std::cout << NowString() << "Try to delete " << vFile2Del << std::endl;
	vDelArg.fsIoStat->Clear(FsOpId::UNLINK);

	try {
		EXPECT_TRUE(vDelArg.fsIoStat->StartOrReStartAccounting());
		UnlinkPath(vDelArg);
		EXPECT_TRUE(vDelArg.fsIoStat->StopIfAccounting());

		std::cout << NowString() << "Succeeded to delete file in "
			<< vDelArg.fsIoStat->AccumulatedMeasure(FsOpId::UNLINK)
			<< " us." << std::endl;
	} catch (std::system_error &e) {
		std::cout << e.what() << std::endl;
		EXPECT_FALSE(true);
	}
}

int CreateFileMain(const std::string &parentDir,
	const std::string &namePrefix,
	int nrFiles2Create)
{
	const int FILE_BYTES = FLAGS_filesize > 0 ? FLAGS_filesize : BYTES_PER_PAGE;
	const int PAGES = FILE_BYTES / BYTES_PER_PAGE;

	try {
		for (int i = 0; i < nrFiles2Create; ++i) {
			CreateFileArg vCreateArg{parentDir + "/" + namePrefix + std::to_string(i)};
			int vFd = CreateFile(vCreateArg);
			for (int pi = 0; pi < PAGES; ++pi) {
				WritePage(vFd, pi);
			}
			CloseFdArg vCloseArg{vFd};
			CloseFd(vCloseArg);
		}
	} catch (std::system_error &e) {
		std::cout << e.what() << std::endl;
		EXPECT_FALSE(true);
		return -1;
	}

	return 0;
}

int DelFileMain(const std::string &parentDir,
	const std::string &namePrefix,
	int nrFiles2Del)
{
	try {
		for (int i = 0; i < nrFiles2Del; ++i) {
			UnlinkPathArg vDelArg{parentDir + "/" + namePrefix + std::to_string(i)};
			UnlinkPath(vDelArg);
		}
	} catch (std::system_error &e) {
		std::cout << e.what() << std::endl;
		EXPECT_FALSE(true);
		return -1;
	}

	return 0;
}

TEST(TSFile, TCCreateFilesT1PDir1)
{
	int vFiles2Create = FLAGS_filenum > 1 ? FLAGS_filenum : 10 * 10000;
	std::string vTestRoot = GetTestRootDir();
	gFsIoStatistics.Clear(FsOpId::CREATE);
	gFsIoStatistics.Clear(FsOpId::CLOSE);

	EXPECT_TRUE(gFsIoStatistics.StartOrReStartAccounting());
	std::cout << NowString() << "Single thread file creation under one directory: begin"
		<< std::endl;
	auto vStart = PerfClock::now();
	int vRc = CreateFileMain(vTestRoot, "file", vFiles2Create);
	EXPECT_EQ(vRc, 0);
	auto vEnd = PerfClock::now();
	std::cout << NowString() <<  "Single thread file creation under one directory: end"
		<< std::endl;
	EXPECT_TRUE(gFsIoStatistics.StopIfAccounting());

	auto vElapsedSecs = (vEnd - vStart) / 1s;
	auto vOps = gFsIoStatistics.OperationsDone(FsOpId::CREATE);
	auto vUs = gFsIoStatistics.AccumulatedMeasure(FsOpId::CREATE);
	std::cout << "file creation operations: " << vOps
		<< "; time-consumed(us): " << vUs
		<< "; average latency(us): " << (vOps ==  0 ? vUs : (vUs / vOps))
		<< "; ops: " << (vElapsedSecs == 0 ? vOps : vOps / vElapsedSecs)
		<< std::endl;
	vOps = gFsIoStatistics.OperationsDone(FsOpId::CLOSE);
	vUs = gFsIoStatistics.AccumulatedMeasure(FsOpId::CLOSE);
	std::cout << "file close operations: " << vOps
		<< "; time-consumed(us): " << vUs
		<< "; average latency(us): " << (vOps ==  0 ? vUs : (vUs / vOps))
		<< "; ops: " << (vElapsedSecs == 0 ? vOps : vOps / vElapsedSecs)
		<< std::endl;
}

class CreateFileFuncObj {
public:
	int operator ()(const std::string &parentDir,
			const std::string &namePrefix,
			int nrFiles2Create)
	{
		return CreateFileMain(parentDir, namePrefix, nrFiles2Create);
	}
};
using CreateFileFuture = std::future<std::invoke_result_t<CreateFileFuncObj,
	  const std::string &, const std::string &, int>>;

class DelFileFuncObj {
public:
	int operator ()(const std::string &parentDir,
			const std::string &namePrefix,
			int nrFiles2Del)
	{
		return DelFileMain(parentDir, namePrefix, nrFiles2Del);
	}
};
using DelFileFuture = std::future<std::invoke_result_t<DelFileFuncObj,
	  const std::string &, const std::string &, int>>;

TEST(TSFile, TCDeleteFilesT1PDir1)
{
	int vFiles2Del = FLAGS_filenum > 1 ? FLAGS_filenum : 10 * 10000;
	std::string vTestRoot = GetTestRootDir();
	gFsIoStatistics.Clear(FsOpId::UNLINK);

	EXPECT_TRUE(gFsIoStatistics.StartOrReStartAccounting());
	std::cout << NowString() << "Single-thread file deletion under one directory: begin"
		<< std::endl;
	auto vStart = PerfClock::now();
	DelFileMain(vTestRoot, "file", vFiles2Del);
	auto vEnd = PerfClock::now();
	std::cout << NowString() << "Single-thread file deletion under one directory: end"
		<< std::endl;
	EXPECT_TRUE(gFsIoStatistics.StopIfAccounting());

	auto vElapsedSecs = (vEnd - vStart) / 1s;
	auto vOps = gFsIoStatistics.OperationsDone(FsOpId::UNLINK);
	auto vUs = gFsIoStatistics.AccumulatedMeasure(FsOpId::UNLINK);
	std::cout << "file deletion operations: " << vOps
		<< "; time-consumed(us): " << vUs
		<< "; average latency(us): " << (vOps ==  0 ? vUs : (vUs / vOps))
		<< "; ops: " << (vElapsedSecs == 0 ? vOps : vOps / vElapsedSecs)
		<< std::endl;
}

TEST(TSFile, TCCreateFilesTxPDir1)
{
	const int NR_FILES2CREATE = FLAGS_filenum > 1 ? FLAGS_filenum : 1000 * 10000;
	const int NR_TASKS = FLAGS_jobnum >= 1 ? FLAGS_jobnum : 4 * HostInfo::OnlineCpus();
	const int NR_FILES_PER_TASK = NR_FILES2CREATE / NR_TASKS;

	std::vector<CreateFileFuture> vFutures(NR_TASKS);
	std::string vTestRoot = GetTestRootDir();
	gFsIoStatistics.Clear(FsOpId::CREATE);
	gFsIoStatistics.Clear(FsOpId::CLOSE);

	EXPECT_TRUE(gFsIoStatistics.StartOrReStartAccounting());
	std::cout << NowString() << "Multi-threaded file creation under one directory: begin"
		<< std::endl;
	auto vStart = PerfClock::now();
	for (int i = 0; i < NR_TASKS; ++i) {
		vFutures[i] = std::async(std::launch::async, CreateFileFuncObj{},
				vTestRoot, "file_t" + std::to_string(i) + "_", NR_FILES_PER_TASK);
	}
	int vFailedThreads = 0;
	for (int i = 0; i < NR_TASKS; ++i) {
		vFailedThreads += vFutures[i].get();
	}
	EXPECT_EQ(vFailedThreads, 0);
	auto vEnd = PerfClock::now();
	std::cout << NowString() << "Multi-threaded file creation under one directory: end"
		<< std::endl;
	EXPECT_TRUE(gFsIoStatistics.StopIfAccounting());

	auto vElapsedSecs = (vEnd - vStart) / 1s;
	auto vOps = gFsIoStatistics.OperationsDone(FsOpId::CREATE);
	auto vUs = gFsIoStatistics.AccumulatedMeasure(FsOpId::CREATE);
	std::cout << "file creation operations: " << vOps
		<< "; time-consumed(us): " << vUs
		<< "; average latency(us): " << (vOps ==  0 ? vUs : (vUs / vOps))
		<< "; ops: " << (vElapsedSecs == 0 ? vOps : vOps / vElapsedSecs)
		<< std::endl;
	vOps = gFsIoStatistics.OperationsDone(FsOpId::CLOSE);
	vUs = gFsIoStatistics.AccumulatedMeasure(FsOpId::CLOSE);
	std::cout << "file close operations: " << vOps
		<< "; time-consumed(us): " << vUs
		<< "; average latency(us): " << (vOps ==  0 ? vUs : (vUs / vOps))
		<< "; ops: " << (vElapsedSecs == 0 ? vOps : vOps / vElapsedSecs)
		<< std::endl;
}

TEST(TSFile, TCDeleteFilesTxPDir1)
{
	const int NR_FILES2CREATE = FLAGS_filenum > 1 ? FLAGS_filenum : 300 * 10000;
	const int NR_TASKS = FLAGS_jobnum >= 1 ? FLAGS_jobnum : 4 * HostInfo::OnlineCpus();
	const int NR_FILES_PER_TASK = NR_FILES2CREATE / NR_TASKS;

	std::vector<DelFileFuture> vFutures(NR_TASKS);
	std::string vTestRoot = GetTestRootDir();
	gFsIoStatistics.Clear(FsOpId::UNLINK);

	EXPECT_TRUE(gFsIoStatistics.StartOrReStartAccounting());
	std::cout << NowString() << "Multi-threaded file deletion under one directory: begin"
		<< std::endl;
	auto vStart = PerfClock::now();
	for (int i = 0; i < NR_TASKS; ++i) {
		vFutures[i] = std::async(std::launch::async, DelFileFuncObj{},
				vTestRoot, "file_t" + std::to_string(i) + "_", NR_FILES_PER_TASK);
	}
	int vFailedThreads = 0;
	for (int i = 0; i < NR_TASKS; ++i) {
		vFailedThreads += vFutures[i].get();
	}
	EXPECT_EQ(vFailedThreads, 0);
	auto vEnd = PerfClock::now();
	std::cout << NowString() << "Multi-threaded file deletion under one directory: end"
		<< std::endl;
	EXPECT_TRUE(gFsIoStatistics.StopIfAccounting());

	auto vElapsedSecs = (vEnd - vStart) / 1s;
	auto vOps = gFsIoStatistics.OperationsDone(FsOpId::UNLINK);
	auto vUs = gFsIoStatistics.AccumulatedMeasure(FsOpId::UNLINK);
	std::cout << "file deletion operations: " << vOps
		<< "; time-consumed(us): " << vUs
		<< "; average latency(us): " << (vOps ==  0 ? vUs : (vUs / vOps))
		<< "; ops: " << (vElapsedSecs == 0 ? vOps : vOps / vElapsedSecs)
		<< std::endl;
}

TEST(TSFile, TCCreateFilesTxPDirx)
{
	const int NR_FILES2CREATE = FLAGS_filenum > 1 ? FLAGS_filenum : 1000 * 10000;
	const int NR_TASKS = FLAGS_jobnum >= 1 ? FLAGS_jobnum : 4 * HostInfo::OnlineCpus();
	const int NR_FILES_PER_TASK = NR_FILES2CREATE / NR_TASKS;

	std::vector<CreateFileFuture> vFutures(NR_TASKS);
	std::string vTestRoot = GetTestRootDir();
	for (int i = 0; i < NR_TASKS; ++i) {
		std::string vTestSubDir = vTestRoot + "/t" + std::to_string(i);
		MkDirArg vArg(vTestSubDir);
		vArg.extraFlags = MkDirArg::IGNORE_EEXIST;
		MkDir(vArg);
	}
	gFsIoStatistics.Clear(FsOpId::CREATE);
	gFsIoStatistics.Clear(FsOpId::CLOSE);

	EXPECT_TRUE(gFsIoStatistics.StartOrReStartAccounting());
	std::cout << NowString() << "Multi-threaded file creation under multiple directories: begin"
		<< std::endl;
	auto vStart = PerfClock::now();
	for (int i = 0; i < NR_TASKS; ++i) {
		std::string vTestSubDir = vTestRoot + "/t" + std::to_string(i);
		vFutures[i] = std::async(std::launch::async, CreateFileFuncObj{},
				vTestSubDir, "file", NR_FILES_PER_TASK);
	}
	int vFailedThreads = 0;
	for (int i = 0; i < NR_TASKS; ++i) {
		vFailedThreads += vFutures[i].get();
	}
	EXPECT_EQ(vFailedThreads, 0);
	auto vEnd = PerfClock::now();
	std::cout << NowString() << "Multi-threaded file creation under multiple directories: end"
		<< std::endl;
	EXPECT_TRUE(gFsIoStatistics.StopIfAccounting());

	auto vElapsedSecs = (vEnd - vStart) / 1s;
	auto vOps = gFsIoStatistics.OperationsDone(FsOpId::CREATE);
	auto vUs = gFsIoStatistics.AccumulatedMeasure(FsOpId::CREATE);
	std::cout << "file creation operations: " << vOps
		<< "; time-consumed(us): " << vUs
		<< "; average latency(us): " << (vOps ==  0 ? vUs : (vUs / vOps))
		<< "; ops: " << (vElapsedSecs == 0 ? vOps : vOps / vElapsedSecs)
		<< std::endl;
	vOps = gFsIoStatistics.OperationsDone(FsOpId::CLOSE);
	vUs = gFsIoStatistics.AccumulatedMeasure(FsOpId::CLOSE);
	std::cout << "file close operations: " << vOps
		<< "; time-consumed(us): " << vUs
		<< "; average latency(us): " << (vOps ==  0 ? vUs : (vUs / vOps))
		<< "; ops: " << (vElapsedSecs == 0 ? vOps : vOps / vElapsedSecs)
		<< std::endl;
}

TEST(TSFile, TCDeleteFilesTxPDirx)
{
	const int NR_FILES2CREATE = FLAGS_filenum > 1 ? FLAGS_filenum : 1000 * 10000;
	const int NR_TASKS = FLAGS_jobnum >= 1 ? FLAGS_jobnum : 4 * HostInfo::OnlineCpus();
	const int NR_FILES_PER_TASK = NR_FILES2CREATE / NR_TASKS;

	std::vector<DelFileFuture> vFutures(NR_TASKS);
	std::string vTestRoot = GetTestRootDir();
	gFsIoStatistics.Clear(FsOpId::UNLINK);

	EXPECT_TRUE(gFsIoStatistics.StartOrReStartAccounting());
	std::cout << NowString() << "Multi-threaded file deletion under multiple directories: begin"
		<< std::endl;
	auto vStart = PerfClock::now();
	for (int i = 0; i < NR_TASKS; ++i) {
		std::string vTestSubDir = vTestRoot + "/t" + std::to_string(i);
		vFutures[i] = std::async(std::launch::async, DelFileFuncObj{},
				vTestSubDir, "file", NR_FILES_PER_TASK);
	}
	int vFailedThreads = 0;
	for (int i = 0; i < NR_TASKS; ++i) {
		vFailedThreads += vFutures[i].get();
	}
	EXPECT_EQ(vFailedThreads, 0);
	auto vEnd = PerfClock::now();
	std::cout << NowString() << "Multi-threaded file deletion under multiple directories: end"
		<< std::endl;
	EXPECT_TRUE(gFsIoStatistics.StopIfAccounting());

	auto vElapsedSecs = (vEnd - vStart) / 1s;
	auto vOps = gFsIoStatistics.OperationsDone(FsOpId::UNLINK);
	auto vUs = gFsIoStatistics.AccumulatedMeasure(FsOpId::UNLINK);
	std::cout << "file deletion operations: " << vOps
		<< "; time-consumed(us): " << vUs
		<< "; average latency(us): " << (vOps ==  0 ? vUs : (vUs / vOps))
		<< "; ops: " << (vElapsedSecs == 0 ? vOps : vOps / vElapsedSecs)
		<< std::endl;
}

