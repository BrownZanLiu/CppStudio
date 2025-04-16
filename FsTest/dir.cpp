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


TEST(TSDirectory, TCMkDirOneShot)
{
	std::string vDir2Mk = GetTestRootDir() + "/subdir";
	MkDirArg vMkDirArg{vDir2Mk};
	std::cout << NowString() << "Try to mkdir: " << vDir2Mk << std::endl;
	vMkDirArg.fsIoStat->Clear(FsOpId::MKDIR);

	try {
		EXPECT_TRUE(vMkDirArg.fsIoStat->StartOrReStartAccounting());
		MkDir(vMkDirArg);
		EXPECT_TRUE(vMkDirArg.fsIoStat->StopIfAccounting());

		std::cout << NowString() << "Succeeded to mkdir in "
			<< vMkDirArg.fsIoStat->AccumulatedMeasure(FsOpId::MKDIR)
			<< " us." << std::endl;
	} catch (std::system_error &e) {
		std::cout << e.what() << std::endl;
		EXPECT_FALSE(true);
	}
}

TEST(TSDirectory, TCRmDirOneShot)
{
	std::string vDir2Rm = GetTestRootDir() + "/subdir";
	RmDirArg vRmDirArg{vDir2Rm};
	std::cout << NowString() << "Try to rmdir: " << vDir2Rm << std::endl;
	vRmDirArg.fsIoStat->Clear(FsOpId::RMDIR);

	try {
		EXPECT_TRUE(vRmDirArg.fsIoStat->StartOrReStartAccounting());
		RmDir(vRmDirArg);
		EXPECT_TRUE(vRmDirArg.fsIoStat->StopIfAccounting());

		std::cout << NowString() << "Succeeded to rmdir in "
			<< vRmDirArg.fsIoStat->AccumulatedMeasure(FsOpId::RMDIR)
			<< " us." << std::endl;
	} catch (std::system_error &e) {
		std::cout << e.what() << std::endl;
		EXPECT_FALSE(true);
	}
}

int MkDirMain(const std::string &parentDir, const std::string &namePrefix, int nrDirs2Mk)
{
	try {
		MkDirArg vMkDirArg{""};
		for (int i = 0; i < nrDirs2Mk; ++i) {
			vMkDirArg.pathname = parentDir + "/" + namePrefix + std::to_string(i);
			MkDir(vMkDirArg);
		}
	} catch (std::system_error &e) {
		std::cout << e.what() << std::endl;
		EXPECT_FALSE(true);
		return -1;
	}

	return 0;
}

int RmDirMain(const std::string &parentDir, const std::string &namePrefix, int nrDirs2Mk)
{
	try {
		RmDirArg vRmDirArg{""};
		for (int i = 0; i < nrDirs2Mk; ++i) {
			vRmDirArg.pathname = parentDir + "/" + namePrefix + std::to_string(i);
			RmDir(vRmDirArg);
		}
	} catch (std::system_error &e) {
		std::cout << e.what() << std::endl;
		EXPECT_FALSE(true);
		return -1;
	}
	return 0;
}

TEST(TSDirectory, TCMkDirsT1PDir1)
{
	int vSubDirs2Mk = FLAGS_dirnum > 1 ? FLAGS_dirnum : 1000 * 10000;
	std::string vTestRoot = GetTestRootDir();
	gFsIoStatistics.Clear(FsOpId::MKDIR);

	EXPECT_TRUE(gFsIoStatistics.StartOrReStartAccounting());
	std::cout << NowString() << "Single thread mkdir under one directory: begin" << std::endl;
	auto vStart = std::chrono::steady_clock::now();
	MkDirMain(vTestRoot, "subdir", vSubDirs2Mk);
	auto vEnd = std::chrono::steady_clock::now();
	std::cout << NowString() << "Single thread mkdir under one directory: end" << std::endl;
	EXPECT_TRUE(gFsIoStatistics.StopIfAccounting());

	auto vElapsedSecs = (vEnd - vStart) / 1s;
	auto vOps = gFsIoStatistics.OperationsDone(FsOpId::MKDIR);
	auto vUs = gFsIoStatistics.AccumulatedMeasure(FsOpId::MKDIR);
	std::cout << "mkdir operations: " << vOps
		<< "; time-consumed(us): " << vUs
		<< "; average latency(us): " << (vOps ==  0 ? vUs : (vUs / vOps))
		<< "; ops: " << (vElapsedSecs == 0 ? vOps : vOps / vElapsedSecs)
		<< std::endl;
}

class MkDirThreadFuncObj {
public:
	int operator ()(const std::string &parentDir, const std::string &namePrefix, int nrDirs2Mk)
	{
		return MkDirMain(parentDir, namePrefix, nrDirs2Mk);
	}
};
using MkDirThreadFuture = std::future<std::invoke_result_t<MkDirThreadFuncObj,
	  const std::string &, const std::string &, int>>;

class RmDirThreadFuncObj {
public:
	int operator ()(const std::string &parentDir, const std::string &namePrefix, int nrDirs2Rm)
	{
		return RmDirMain(parentDir, namePrefix, nrDirs2Rm);
	}
};
using RmDirThreadFuture = std::future<std::invoke_result_t<RmDirThreadFuncObj,
	  const std::string &, const std::string &, int>>;

TEST(TSDirectory, TCRmDirsT1PDir1)
{
	int vSubDirs2Mk = FLAGS_dirnum > 1 ? FLAGS_dirnum : 1000 * 10000;
	std::string vTestRoot = GetTestRootDir();
	gFsIoStatistics.Clear(FsOpId::RMDIR);

	EXPECT_TRUE(gFsIoStatistics.StartOrReStartAccounting());
	std::cout << NowString() << "Single-thread rmdir under one directory: begin" << std::endl;
	auto vStart = std::chrono::steady_clock::now();
	RmDirMain(vTestRoot, "subdir", vSubDirs2Mk);
	auto vEnd = std::chrono::steady_clock::now();
	std::cout << NowString() << "Single-thread rmdir under one directory: end" << std::endl;
	EXPECT_TRUE(gFsIoStatistics.StopIfAccounting());

	auto vElapsedSecs = (vEnd - vStart) / 1s;
	auto vOps = gFsIoStatistics.OperationsDone(FsOpId::RMDIR);
	auto vUs = gFsIoStatistics.AccumulatedMeasure(FsOpId::RMDIR);
	std::cout << "rmdir operations: " << vOps
		<< "; time-consumed(us): " << vUs
		<< "; average latency(us): " << (vOps ==  0 ? vUs : (vUs / vOps))
		<< "; ops: " << (vElapsedSecs == 0 ? vOps : vOps / vElapsedSecs)
		<< std::endl;
}

TEST(TSDirectory, TCMkDirsTxPDir1)
{
	const int NR_DIRS2MK = FLAGS_dirnum > 1 ? FLAGS_dirnum : 1000 * 10000;
	const int NR_TASKS = FLAGS_jobnum > 1 ? FLAGS_jobnum : 4 * HostInfo::OnlineCpus();
	const int NR_DIRS_PER_TASK = NR_DIRS2MK / NR_TASKS;

	std::vector<MkDirThreadFuture> vFutures(NR_TASKS);
	std::string vTestRoot = GetTestRootDir();
	gFsIoStatistics.Clear(FsOpId::MKDIR);

	EXPECT_TRUE(gFsIoStatistics.StartOrReStartAccounting());
	std::cout << NowString() << "Multi-thread mkdir under one directory: begin" << std::endl;
	auto vStart = std::chrono::steady_clock::now();
	for (int i = 0; i < NR_TASKS; ++i) {
		vFutures[i] = std::async(std::launch::async, MkDirThreadFuncObj{},
				vTestRoot, "subdir_t" + std::to_string(i) + "_", NR_DIRS_PER_TASK);
	}
	int vFailedThreads = 0;
	for (int i = 0; i < NR_TASKS; ++i) {
		vFailedThreads += vFutures[i].get();
	}
	EXPECT_EQ(vFailedThreads, 0);
	auto vEnd = std::chrono::steady_clock::now();
	std::cout << NowString() << "Multi-thread mkdir under one directory: end" << std::endl;
	EXPECT_TRUE(gFsIoStatistics.StopIfAccounting());

	auto vElapsedSecs = (vEnd - vStart) / 1s;
	auto vOps = gFsIoStatistics.OperationsDone(FsOpId::MKDIR);
	auto vUs = gFsIoStatistics.AccumulatedMeasure(FsOpId::MKDIR);
	std::cout << "mkdir operations: " << vOps
		<< "; time-consumed(us): " << vUs
		<< "; average latency(us): " << (vOps ==  0 ? vUs : (vUs / vOps))
		<< "; ops: " << (vElapsedSecs == 0 ? vOps : vOps / vElapsedSecs)
		<< std::endl;
}

TEST(TSDirectory, TCRmDirsTxPDir1)
{
	const int NR_DIRS2MK = FLAGS_dirnum > 1 ? FLAGS_dirnum : 1000 * 10000;
	const int NR_TASKS = FLAGS_jobnum > 1 ? FLAGS_jobnum : 4 * HostInfo::OnlineCpus();
	const int NR_DIRS_PER_TASK = NR_DIRS2MK / NR_TASKS;

	std::vector<RmDirThreadFuture> vFutures(NR_TASKS);
	std::string vTestRoot = GetTestRootDir();
	gFsIoStatistics.Clear(FsOpId::RMDIR);

	EXPECT_TRUE(gFsIoStatistics.StartOrReStartAccounting());
	std::cout << NowString() << "Multi-thread rmdir under one directory: begin" << std::endl;
	auto vStart = std::chrono::steady_clock::now();
	for (int i = 0; i < NR_TASKS; ++i) {
		vFutures[i] = std::async(std::launch::async, RmDirThreadFuncObj{},
				vTestRoot, "subdir_t" + std::to_string(i) + "_", NR_DIRS_PER_TASK);
	}
	int vFailedThreads = 0;
	for (int i = 0; i < NR_TASKS; ++i) {
		vFailedThreads += vFutures[i].get();
	}
	EXPECT_EQ(vFailedThreads, 0);
	auto vEnd = std::chrono::steady_clock::now();
	std::cout << NowString() << "Multi-thread rmdir under one directory: end" << std::endl;
	EXPECT_TRUE(gFsIoStatistics.StopIfAccounting());

	auto vElapsedSecs = (vEnd - vStart) / 1s;
	auto vOps = gFsIoStatistics.OperationsDone(FsOpId::RMDIR);
	auto vUs = gFsIoStatistics.AccumulatedMeasure(FsOpId::RMDIR);
	std::cout << "rmdir operations: " << vOps
		<< "; time-consumed(us): " << vUs
		<< "; average latency(us): " << (vOps ==  0 ? vUs : (vUs / vOps))
		<< "; ops: " << (vElapsedSecs == 0 ? vOps : vOps / vElapsedSecs)
		<< std::endl;
}

TEST(TSDirectory, TCMkDirsTxPDirx)
{
	const int NR_DIRS2MK = FLAGS_dirnum > 1 ? FLAGS_dirnum : 1000 * 10000;
	const int NR_TASKS = FLAGS_jobnum > 1 ? FLAGS_jobnum : 4 * HostInfo::OnlineCpus();
	const int NR_DIRS_PER_TASK = NR_DIRS2MK / NR_TASKS;

	std::vector<MkDirThreadFuture> vFutures(NR_TASKS);
	std::string vTestRoot = GetTestRootDir();
	for (int i = 0; i < NR_TASKS; ++i) {
		std::string vTestSubDir = vTestRoot + "/t" + std::to_string(i);
		MkDirArg vArg(vTestSubDir);
		MkDir(vArg);
	}
	gFsIoStatistics.Clear(FsOpId::MKDIR);

	EXPECT_TRUE(gFsIoStatistics.StartOrReStartAccounting());
	std::cout << NowString() << "Multi-thread mkdir under multiple directories: begin"
		<< std::endl;
	auto vStart = std::chrono::steady_clock::now();
	for (int i = 0; i < NR_TASKS; ++i) {
		std::string vTestSubDir = vTestRoot + "/t" + std::to_string(i);
		vFutures[i] = std::async(std::launch::async, MkDirThreadFuncObj{},
				vTestSubDir, "subdir", NR_DIRS_PER_TASK);
	}
	int vFailedThreads = 0;
	for (int i = 0; i < NR_TASKS; ++i) {
		vFailedThreads += vFutures[i].get();
	}
	EXPECT_EQ(vFailedThreads, 0);
	auto vEnd = std::chrono::steady_clock::now();
	std::cout << NowString() << "Multi-thread mkdir under multiple directories: end"
		<< std::endl;
	EXPECT_TRUE(gFsIoStatistics.StopIfAccounting());

	auto vElapsedSecs = (vEnd - vStart) / 1s;
	auto vOps = gFsIoStatistics.OperationsDone(FsOpId::MKDIR);
	auto vUs = gFsIoStatistics.AccumulatedMeasure(FsOpId::MKDIR);
	std::cout << "mkdir operations: " << vOps
		<< "; time-consumed(us): " << vUs
		<< "; average latency(us): " << (vOps ==  0 ? vUs : (vUs / vOps))
		<< "; ops: " << (vElapsedSecs == 0 ? vOps : vOps / vElapsedSecs)
		<< std::endl;
}

TEST(TSDirectory, TCRmDirsTxPDirx)
{
	const int NR_DIRS2MK = FLAGS_dirnum > 1 ? FLAGS_dirnum : 1000 * 10000;
	const int NR_TASKS = FLAGS_jobnum > 1 ? FLAGS_jobnum : 4 * HostInfo::OnlineCpus();
	const int NR_DIRS_PER_TASK = NR_DIRS2MK / NR_TASKS;

	std::vector<RmDirThreadFuture> vFutures(NR_TASKS);
	std::string vTestRoot = GetTestRootDir();
	gFsIoStatistics.Clear(FsOpId::RMDIR);

	EXPECT_TRUE(gFsIoStatistics.StartOrReStartAccounting());
	std::cout << NowString() << "Multi-thread rmdir under multiple directories: begin"
		<< std::endl;
	auto vStart = std::chrono::steady_clock::now();
	for (int i = 0; i < NR_TASKS; ++i) {
		std::string vTestSubDir = vTestRoot + "/t" + std::to_string(i);
		vFutures[i] = std::async(std::launch::async, RmDirThreadFuncObj{},
				vTestSubDir, "subdir", NR_DIRS_PER_TASK);
	}
	int vFailedThreads = 0;
	for (int i = 0; i < NR_TASKS; ++i) {
		vFailedThreads += vFutures[i].get();
	}
	EXPECT_EQ(vFailedThreads, 0);
	auto vEnd = std::chrono::steady_clock::now();
	std::cout << NowString() << "Multi-thread rmdir under multiple directories: end"
		<< std::endl;
	EXPECT_TRUE(gFsIoStatistics.StopIfAccounting());

	auto vElapsedSecs = (vEnd - vStart) / 1s;
	auto vOps = gFsIoStatistics.OperationsDone(FsOpId::RMDIR);
	auto vUs = gFsIoStatistics.AccumulatedMeasure(FsOpId::RMDIR);
	std::cout << "rmdir operations: " << vOps
		<< "; time-consumed(us): " << vUs
		<< "; average latency(us): " << (vOps ==  0 ? vUs : (vUs / vOps))
		<< "; ops: " << (vElapsedSecs == 0 ? vOps : vOps / vElapsedSecs)
		<< std::endl;
}

