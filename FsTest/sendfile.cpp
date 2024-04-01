#include <iostream>
#include <future>  // std::async, std::future, etc
#include <string>

#include <errno.h>
#include <fcntl.h>  // open()
#include <string.h>  // strerror()
#include <sys/sendfile.h>  // sendfile()
#include <sys/stat.h>  // mkdir()

#include <gflags/gflags.h>
#include <gtest/gtest.h>

#include "fstest_flags.h"

namespace liuzan {
namespace fstest {
namespace sendfile {

void MakeTestDir(const std::string & root)
{
	std::string vTestDir = root + "/sendfile";
	int dirFd = mkdir(vTestDir.c_str(), S_IRWXU);
	if (dirFd == 0 || errno == EEXIST) return;

	throw std::string("FileSystem API exception: ") + strerror(errno);
}

}  // namespace sendfile
}  // namespace fstest
}  // namespace liuzan

TEST(TSSendfile, TCSingleShot)
{
	using namespace liuzan::fstest::sendfile;

	MakeTestDir(FLAGS_root);
}

