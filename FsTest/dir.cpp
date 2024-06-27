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

using namespace liuzan;
using namespace liuzan::fstest;
using namespace liuzan::filesystem;
using namespace std::literals;


TEST(TSDirectory, TCMkDirOneShot)
{
	std::string vDir2Mk = GetTestRootDir() + "/subdir";
	MkDirArg vMkDirArg{vDir2Mk};
	std::cout << NowString() << "Try to mkdir: " << vDir2Mk << std::endl;
	vMkDirArg.fsIoStat->Clear(filesystem_operation_id::MKDIR);

	try {
		EXPECT_TRUE(vMkDirArg.fsIoStat->StartOrReStartAccounting());
		MkDir(vMkDirArg);
		EXPECT_TRUE(vMkDirArg.fsIoStat->StopIfAccounting());

		std::cout << NowString() << "Succeeded to mkdir in "
			      << vMkDirArg.fsIoStat->AccumulatedMeasure(filesystem_operation_id::MKDIR)
			      << " us."
				  << std::endl;
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
	vRmDirArg.fsIoStat->Clear(filesystem_operation_id::RMDIR);

	try {
		EXPECT_TRUE(vRmDirArg.fsIoStat->StartOrReStartAccounting());
		RmDir(vRmDirArg);
		EXPECT_TRUE(vRmDirArg.fsIoStat->StopIfAccounting());

		std::cout << NowString() << "Succeeded to rmdir in "
			      << vRmDirArg.fsIoStat->AccumulatedMeasure(filesystem_operation_id::RMDIR)
			      << " us."
				  << std::endl;
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
	gFsIoStatics.Clear(filesystem_operation_id::MKDIR);

	EXPECT_TRUE(gFsIoStatics.StartOrReStartAccounting());
	std::cout << NowString() <<  "Single thread mkdir under one directory: begin" << std::endl;
	auto vStart = std::chrono::steady_clock::now();
	MkDirMain(vTestRoot, "subdir", vSubDirs2Mk);
	auto vEnd = std::chrono::steady_clock::now();
	std::cout << NowString() <<  "Single thread mkdir under one directory: end" << std::endl;
	EXPECT_TRUE(gFsIoStatics.StopIfAccounting());

	auto vElapsedSecs = (vEnd - vStart) / 1s;
	auto vOps = gFsIoStatics.OperationsDone(filesystem_operation_id::MKDIR);
	auto vUs = gFsIoStatics.AccumulatedMeasure(filesystem_operation_id::MKDIR);
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
	gFsIoStatics.Clear(filesystem_operation_id::RMDIR);

	EXPECT_TRUE(gFsIoStatics.StartOrReStartAccounting());
	std::cout << NowString() <<  "Single-thread rmdir under one directory: begin" << std::endl;
	auto vStart = std::chrono::steady_clock::now();
	RmDirMain(vTestRoot, "subdir", vSubDirs2Mk);
	auto vEnd = std::chrono::steady_clock::now();
	std::cout << NowString() <<  "Single-thread rmdir under one directory: end" << std::endl;
	EXPECT_TRUE(gFsIoStatics.StopIfAccounting());

	auto vElapsedSecs = (vEnd - vStart) / 1s;
	auto vOps = gFsIoStatics.OperationsDone(filesystem_operation_id::RMDIR);
	auto vUs = gFsIoStatics.AccumulatedMeasure(filesystem_operation_id::RMDIR);
	std::cout << "rmdir operations: " << vOps
		      << "; time-consumed(us): " << vUs
			  << "; average latency(us): " << (vOps ==  0 ? vUs : (vUs / vOps))
			  << "; ops: " << (vElapsedSecs == 0 ? vOps : vOps / vElapsedSecs)
			  << std::endl;
}

TEST(TSDirectory, TCMkDirsTxPDir1)
{
	const int kSubDirs2Mk = FLAGS_dirnum > 1 ? FLAGS_dirnum : 1000 * 10000;
	const int kNrTasks = FLAGS_jobnum > 1 ? FLAGS_jobnum : 4 * HostInfo::OnlineCpus();
	const int kSubDirsPerTask = kSubDirs2Mk / kNrTasks;

	std::vector<MkDirThreadFuture> vFutures(kNrTasks);
	std::string vTestRoot = GetTestRootDir();
	gFsIoStatics.Clear(filesystem_operation_id::MKDIR);

	EXPECT_TRUE(gFsIoStatics.StartOrReStartAccounting());
	std::cout << NowString() <<  "Multi-thread mkdir under one directory: begin" << std::endl;
	auto vStart = std::chrono::steady_clock::now();
	for (int i = 0; i < kNrTasks; ++i) {
		vFutures[i] = std::async(std::launch::async, MkDirThreadFuncObj{},
				vTestRoot, "subdir_t" + std::to_string(i) + "_", kSubDirsPerTask);
	}
	int vFailedThreads = 0;
	for (int i = 0; i < kNrTasks; ++i) {
		vFailedThreads += vFutures[i].get();
	}
	EXPECT_EQ(vFailedThreads, 0);
	auto vEnd = std::chrono::steady_clock::now();
	std::cout << NowString() <<  "Multi-thread mkdir under one directory: end" << std::endl;
	EXPECT_TRUE(gFsIoStatics.StopIfAccounting());

	auto vElapsedSecs = (vEnd - vStart) / 1s;
	auto vOps = gFsIoStatics.OperationsDone(filesystem_operation_id::MKDIR);
	auto vUs = gFsIoStatics.AccumulatedMeasure(filesystem_operation_id::MKDIR);
	std::cout << "mkdir operations: " << vOps
		      << "; time-consumed(us): " << vUs
			  << "; average latency(us): " << (vOps ==  0 ? vUs : (vUs / vOps))
			  << "; ops: " << (vElapsedSecs == 0 ? vOps : vOps / vElapsedSecs)
			  << std::endl;
}

TEST(TSDirectory, TCRmDirsTxPDir1)
{
	const int kSubDirs2Mk = FLAGS_dirnum > 1 ? FLAGS_dirnum : 1000 * 10000;
	const int kNrTasks = FLAGS_jobnum > 1 ? FLAGS_jobnum : 4 * HostInfo::OnlineCpus();
	const int kSubDirsPerTask = kSubDirs2Mk / kNrTasks;

	std::vector<RmDirThreadFuture> vFutures(kNrTasks);
	std::string vTestRoot = GetTestRootDir();
	gFsIoStatics.Clear(filesystem_operation_id::RMDIR);

	EXPECT_TRUE(gFsIoStatics.StartOrReStartAccounting());
	std::cout << NowString() <<  "Multi-thread rmdir under one directory: begin" << std::endl;
	auto vStart = std::chrono::steady_clock::now();
	for (int i = 0; i < kNrTasks; ++i) {
		vFutures[i] = std::async(std::launch::async, RmDirThreadFuncObj{},
				vTestRoot, "subdir_t" + std::to_string(i) + "_", kSubDirsPerTask);
	}
	int vFailedThreads = 0;
	for (int i = 0; i < kNrTasks; ++i) {
		vFailedThreads += vFutures[i].get();
	}
	EXPECT_EQ(vFailedThreads, 0);
	auto vEnd = std::chrono::steady_clock::now();
	std::cout << NowString() <<  "Multi-thread rmdir under one directory: end" << std::endl;
	EXPECT_TRUE(gFsIoStatics.StopIfAccounting());

	auto vElapsedSecs = (vEnd - vStart) / 1s;
	auto vOps = gFsIoStatics.OperationsDone(filesystem_operation_id::RMDIR);
	auto vUs = gFsIoStatics.AccumulatedMeasure(filesystem_operation_id::RMDIR);
	std::cout << "rmdir operations: " << vOps
		      << "; time-consumed(us): " << vUs
			  << "; average latency(us): " << (vOps ==  0 ? vUs : (vUs / vOps))
			  << "; ops: " << (vElapsedSecs == 0 ? vOps : vOps / vElapsedSecs)
			  << std::endl;
}

TEST(TSDirectory, TCMkDirsTxPDirx)
{
	const int kSubDirs2Mk = FLAGS_dirnum > 1 ? FLAGS_dirnum : 1000 * 10000;
	const int kNrTasks = FLAGS_jobnum > 1 ? FLAGS_jobnum : 4 * HostInfo::OnlineCpus();
	const int kSubDirsPerTask = kSubDirs2Mk / kNrTasks;

	std::vector<MkDirThreadFuture> vFutures(kNrTasks);
	std::string vTestRoot = GetTestRootDir();
	for (int i = 0; i < kNrTasks; ++i) {
		std::string vTestSubDir = vTestRoot + "/t" + std::to_string(i);
		MkDirArg vArg(vTestSubDir, MkDirArg::IGNORE_EEXIST | MkDirArg::NO_ACCOUNTING);
		MkDir(vArg);
	}
	gFsIoStatics.Clear(filesystem_operation_id::MKDIR);

	EXPECT_TRUE(gFsIoStatics.StartOrReStartAccounting());
	std::cout << NowString() <<  "Multi-thread mkdir under multiple directories: begin" << std::endl;
	auto vStart = std::chrono::steady_clock::now();
	for (int i = 0; i < kNrTasks; ++i) {
		std::string vTestSubDir = vTestRoot + "/t" + std::to_string(i);
		vFutures[i] = std::async(std::launch::async, MkDirThreadFuncObj{},
				vTestSubDir, "subdir", kSubDirsPerTask);
	}
	int vFailedThreads = 0;
	for (int i = 0; i < kNrTasks; ++i) {
		vFailedThreads += vFutures[i].get();
	}
	EXPECT_EQ(vFailedThreads, 0);
	auto vEnd = std::chrono::steady_clock::now();
	std::cout << NowString() <<  "Multi-thread mkdir under multiple directories: end" << std::endl;
	EXPECT_TRUE(gFsIoStatics.StopIfAccounting());

	auto vElapsedSecs = (vEnd - vStart) / 1s;
	auto vOps = gFsIoStatics.OperationsDone(filesystem_operation_id::MKDIR);
	auto vUs = gFsIoStatics.AccumulatedMeasure(filesystem_operation_id::MKDIR);
	std::cout << "mkdir operations: " << vOps
		      << "; time-consumed(us): " << vUs
			  << "; average latency(us): " << (vOps ==  0 ? vUs : (vUs / vOps))
			  << "; ops: " << (vElapsedSecs == 0 ? vOps : vOps / vElapsedSecs)
			  << std::endl;
}

TEST(TSDirectory, TCRmDirsTxPDirx)
{
	const int kSubDirs2Mk = FLAGS_dirnum > 1 ? FLAGS_dirnum : 1000 * 10000;
	const int kNrTasks = FLAGS_jobnum > 1 ? FLAGS_jobnum : 4 * HostInfo::OnlineCpus();
	const int kSubDirsPerTask = kSubDirs2Mk / kNrTasks;

	std::vector<RmDirThreadFuture> vFutures(kNrTasks);
	std::string vTestRoot = GetTestRootDir();
	for (int i = 0; i < kNrTasks; ++i) {
		std::string vTestSubDir = vTestRoot + "/t" + std::to_string(i);
		MkDirArg vArg(vTestSubDir, MkDirArg::IGNORE_EEXIST | MkDirArg::NO_ACCOUNTING);
		MkDir(vArg);
	}
	gFsIoStatics.Clear(filesystem_operation_id::RMDIR);

	EXPECT_TRUE(gFsIoStatics.StartOrReStartAccounting());
	std::cout << NowString() <<  "Multi-thread rmdir under multiple directories: begin" << std::endl;
	auto vStart = std::chrono::steady_clock::now();
	for (int i = 0; i < kNrTasks; ++i) {
		std::string vTestSubDir = vTestRoot + "/t" + std::to_string(i);
		vFutures[i] = std::async(std::launch::async, RmDirThreadFuncObj{},
				vTestSubDir, "subdir", kSubDirsPerTask);
	}
	int vFailedThreads = 0;
	for (int i = 0; i < kNrTasks; ++i) {
		vFailedThreads += vFutures[i].get();
	}
	EXPECT_EQ(vFailedThreads, 0);
	auto vEnd = std::chrono::steady_clock::now();
	std::cout << NowString() <<  "Multi-thread rmdir under multiple directories: end" << std::endl;
	EXPECT_TRUE(gFsIoStatics.StopIfAccounting());

	auto vElapsedSecs = (vEnd - vStart) / 1s;
	auto vOps = gFsIoStatics.OperationsDone(filesystem_operation_id::RMDIR);
	auto vUs = gFsIoStatics.AccumulatedMeasure(filesystem_operation_id::RMDIR);
	std::cout << "rmdir operations: " << vOps
		      << "; time-consumed(us): " << vUs
			  << "; average latency(us): " << (vOps ==  0 ? vUs : (vUs / vOps))
			  << "; ops: " << (vElapsedSecs == 0 ? vOps : vOps / vElapsedSecs)
			  << std::endl;
}

