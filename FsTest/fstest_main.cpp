#include <iostream>

#include <gflags/gflags.h>

DEFINE_string(test_rootpath, "/mnt/fstest", "Specify the root directory where fstest will run.");

int main(int argc, char **argv)
{
	std::cout << "Root directory to run fstest: " << FLAGS_test_rootpath << std::endl;

	return 0;
}
