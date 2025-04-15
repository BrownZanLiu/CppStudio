#ifndef LIUZAN_PERF_H
#define LIUZAN_PERF_H

#include <atomic>  // std::atomic_uint64_t
#include <chrono>  // std::literals
#include <cstdint>  // uint64_t
#include <thread>  // std::this_thead::sleep_for

#include <sched.h>  // sched_getcpu()

#include <percpu_var.h>
#include <host_info.h>

namespace liuzan {
using namespace std::literals;

struct PerfCounter {
#if defined(PERF_WITH_ATOMIC_COUNTERS)
	std::atomic_uint64_t operations;
	std::atomic_uint64_t accumulated;
#else
	uint64_t operations;
	uint64_t accumulated;
#endif
};

using PerfClock = std::chrono::steady_clock;
using PerfTimePoint = std::chrono::time_point<PerfClock>;

namespace filesystem {

enum class FsOpId {
	WRITE = 0,  // write()/writev()/pwrite64()/pwritev()/pwritev2()
	READ = 1,  // read()/readv()/pread64()/preadv()/preadv2()
	MMAP = 2,  // mmap()/mmap2()
	MUNMAP = 3,  // munmap()
	MKDIR = 4,  // mkdir()/mkdirat()
	RMDIR = 5,  // rmdir()
	CREAT = 6,  // creat()/open()/mknod()/mknodat()
	CREATE = CREAT,  // preferred
	RMFILE = 7,
	RENAME = 8,  // rename()/renameat()/renameat2()
	OPEN = 9,  // open()/openat()/openat2()
	CLOSE = 10,  // close()
	FLOCK = 11,  // flock()
	ACCESS = 12,  // access()/faccessat()/faccessat2()
	STAT = 13,  // fstat()/fstat64()/fstatat64()/lstat()/lstat64()/newfstatat()/stat()/stat64()/statx()
	CHMOD = 14,  // chmod()/fchmod()/fchmodat()
	CHOWN = 15,  // chown()/fchown()/fchownat()/lchown()
	UTIMES = 16,  // futimesat()/utime()/utimes()/utimensat()
	LINK = 17,  // link()/linkat()
	UNLINK = 18,  // unlink()/unlinkat()
	SETXATTR = 19,  // fsetxattr()/setxattr()/lsetxattr()
	GETXATTR = 20,  // fgetxatrr()/getxattr()/lgetxattr()
	REMOVEXATTR = 21,  // fremovexattr()/lremovexattr()
	IDS_TO_PERF,

	LISTXATTR,  // flistxattr()/listxattr()/llistxattr()
	GETDENTS,  // getdents()/getdents64()/readdir()
	READLINK,  // readlink()/readlinkat()

	MOUNT,  // mount()
	UMOUNT,  // umount()/umount2()
	FSTATFS,  // fstatfs()/fstatfs64()/statfs()/statfs64()/ustat()
	FDATASYNC,  // fdatasync()
	FSYNC,  // fsync()/sync_file_ranges()/sync_file_ranges2()
	SYNCFS,  // sync()/syncfs()
	FTRUNCATE,  // ftruncate()/ftruncate64()/truncate()/truncate64()
	FALLOCATE,  // fallocate()
	FADVISE,  // fadvise64()/fadvise64_64()
	MADVISE,  // madvise()
	LSEEK,  // lseak
	DUP,  // dup()/dup2()/dup3()
	CLOSE_RANGE,  // close_range()
	COPY_FILE_RANGE,  // copy_file_range()
	FCNTL,  // fcntl()/fcntl64()
	CHDIR,  // chdir()/fchdir()
	GETCWD,  // getcwd()
	QUOTACTL,  // quotactl()
	READAHEAD,  // readahead()
	SENDFILE,  // sendfile()/sendfile64()
};

struct FsPerfCounters {
	static constexpr int MAX_OPERATIONS2COUNT = static_cast<int>(FsOpId::IDS_TO_PERF);
	PerfCounter counters[MAX_OPERATIONS2COUNT];
};

class FsIoStatistics: PerCpuVar<FsPerfCounters> {
/**
 * For latency, use microsecond as unit.
 * For bandwidth, use byte as unit.
 */
public:
	FsIoStatistics()
	{
		clearAll();
		finishInit();
	}

	void IncOperations(FsOpId id)
	{
		curCpuPart().counters[static_cast<int>(id)].operations++;
	}

	void AddMeasure(FsOpId id, uint64_t measure)
	{
		curCpuPart().counters[static_cast<int>(id)].accumulated += measure;
	}

	uint64_t OperationsDone(FsOpId id)
	{
		uint64_t vSysTotal = 0;

		for (int i = 0; i < nrOfCpus(); ++i) {
			vSysTotal += cpuPart(i).counters[static_cast<int>(id)].operations;
		}

		return vSysTotal;
	}

	uint64_t AccumulatedMeasure(FsOpId id)
	{
		uint64_t vSysTotal = 0;

		for (int i = 0; i < nrOfCpus(); ++i) {
			vSysTotal += cpuPart(i).counters[static_cast<int>(id)].accumulated;
		}

		return vSysTotal;
	}

	void Clear(FsOpId id)
	{
		for (int i = 0; i < nrOfCpus(); ++i) {
			cpuPart(i).counters[static_cast<int>(id)].operations = 0;
			cpuPart(i).counters[static_cast<int>(id)].accumulated = 0;
		}
	}

#if defined(PERF_WITH_SYNCPOINTS)
	bool IsPerfDone()
	{
		int vSleepMs = 0;
		do {
			std::atomic_thread_fence(std::memory_order_acquire);
			std::this_thread::sleep_for(1ms);
			++vSleepMs;
			if (vSleepMs > 10) {
				return false;
			}
		} while(state != fs_perf_state::DONE);
		return true;
	}

	bool StartAccountingIfReady()
	{
		int vSleepMs = 0;
		do {
			std::atomic_thread_fence(std::memory_order_acquire);
			std::this_thread::sleep_for(1ms);
			++vSleepMs;
			if (vSleepMs > 10) {
				return false;
			}
		} while(state != fs_perf_state::READY2ACCOUNT);

		state = fs_perf_state::ACCOUNTING;

		std::atomic_thread_fence(std::memory_order_release);
		return true;
	}

	bool StopIfAccounting()
	{
		int vSleepMs = 0;
		do {
			std::atomic_thread_fence(std::memory_order_acquire);
			std::this_thread::sleep_for(1ms);
			++vSleepMs;
			if (vSleepMs > 10) {
				return false;
			}
		} while(state != fs_perf_state::ACCOUNTING);

		state = fs_perf_state::DONE;

		std::atomic_thread_fence(std::memory_order_release);

		return true;
	}

	bool RestartAccountingIfDone()
	{
		int vSleepMs = 0;
		do {
			std::atomic_thread_fence(std::memory_order_acquire);
			std::this_thread::sleep_for(1ms);
			++vSleepMs;
			if (vSleepMs > 10) {
				return false;
			}
		} while(state != fs_perf_state::DONE);

		state = fs_perf_state::ACCOUNTING;
		std::atomic_thread_fence(std::memory_order_release);

		return true;
	}

	bool StartOrReStartAccounting()
	{
		int vSleepMs = 0;
		do {
			std::atomic_thread_fence(std::memory_order_acquire);
			std::this_thread::sleep_for(1ms);
			++vSleepMs;
			if (vSleepMs > 10) {
				return false;
			}
		} while(state != fs_perf_state::DONE && state != fs_perf_state::READY2ACCOUNT);

		state = fs_perf_state::ACCOUNTING;
		std::atomic_thread_fence(std::memory_order_release);

		return true;
	}
#else
	bool IsPerfDone()
	{
		return true;
	}

	bool StartAccountingIfReady()
	{
		return true;
	}

	bool StopIfAccounting()
	{
		return true;
	}

	bool RestartAccountingIfDone()
	{
		return true;
	}

	bool StartOrReStartAccounting()
	{
		return true;
	}
#endif

private:
#if defined(PERF_WITH_SYNCPOINTS)
	void finishInit()
	{
		state = fs_perf_state::READY2ACCOUNT;
		std::atomic_thread_fence(std::memory_order_release);
	}

	enum class fs_perf_state: uint64_t {
		INIT = 1 << 0,
		READY2ACCOUNT = 1 << 1,
		ACCOUNTING = 1 << 2,
		DONE = 1 << 3,
	};
	std::atomic<fs_perf_state> state;
#else
    void finishInit() {}
#endif
};

}  // namespace filesystem

}  // namespace liuzan

#endif  // LIUZAN_PERF_H
