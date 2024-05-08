#ifndef FSTEST_LIBFSTEST_H
#define FSTEST_LIBFSTEST_H

#include <chrono>  // std::time_point
#include <cstdint>  // uint64_t
#include <iostream>
#include <string>


namespace liuzan {
namespace fstest {

using SystemClock = std::chrono::system_clock;
using SystemTime = std::chrono::time_point<SystemClock>;
using namespace liuzan::filesystem;

SystemTime Now();

std::string NowString();

void * GetPageData(uint32_t pageNum);

void WritePage(int fd, uint32_t pageIndex);

}  // namespace fstest
}  // namespace liuzan

#endif  // FSTEST_LIBFSTEST_H
