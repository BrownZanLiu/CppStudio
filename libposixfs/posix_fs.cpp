#include <chrono>  // PerfClock
#include <string>  // std::to_string()
#include <system_error>

#include <file_system.h>

namespace liuzan {
namespace filesystem {

FsIoStatistics gFsIoStatistics;

void MkDir(const std::string &dirpath, mode_t mode)
{
	int vRc = mkdir(dirpath.c_str(), mode);

	if (vRc != 0 && errno != EEXIST) {
		throw std::system_error(errno, std::system_category(),
			"Failed to mkdir " + dirpath);
	}
}

void MkDir(MkDirArg &mkdirArg)
{
	using namespace std::literals;

	PerfTimePoint vTpStart, vTpEnd;
	int vRc;

	if (mkdirArg.extraFlags & MkDirArg::DO_ACCOUNTING) {
		vTpStart = PerfClock::now();
		vRc = mkdirat(mkdirArg.parentDirFd, mkdirArg.pathname.c_str(), mkdirArg.mode);
		vTpEnd = PerfClock::now();
	} else {
		vRc = mkdirat(mkdirArg.parentDirFd, mkdirArg.pathname.c_str(), mkdirArg.mode);
	}

	// 0 vs. -1
	if (vRc != 0) {
		if (errno != EEXIST || (mkdirArg.extraFlags & MkDirArg::THROW_EEXIST)) {
			throw std::system_error(errno, std::system_category(),
				"Failed to mkdir " + mkdirArg.pathname);
		}
	} else if ((mkdirArg.extraFlags & MkDirArg::DO_ACCOUNTING) &&
		   mkdirArg.fsIoStat != nullptr) {
		const auto vUs = static_cast<uint64_t>((vTpEnd - vTpStart) / 1us);
		mkdirArg.fsIoStat->IncOperations(FsOpId::MKDIR);
		mkdirArg.fsIoStat->AddMeasure(FsOpId::MKDIR, vUs);
	}
}

void RmDir(const std::string &dirpath)
{
	int vRc = rmdir(dirpath.c_str());

	if (vRc != 0 && errno != ENOENT) {
		throw std::system_error(errno, std::system_category(),
			"Failed to rmdir " + dirpath);
	}
}

void RmDir(RmDirArg &rmdirArg)
{
	using namespace std::literals;

	PerfTimePoint vTpStart, vTpEnd;
	int vRc = 0;

	if (rmdirArg.extraFlags & RmDirArg::DO_ACCOUNTING) {
		vTpStart = PerfClock::now();
		vRc = rmdir(rmdirArg.pathname.c_str());
		vTpEnd = PerfClock::now();
	} else {
		vRc = rmdir(rmdirArg.pathname.c_str());
	}

	/* 0 vs. -1 */
	if (vRc != 0) {
		if (errno != ENOENT || (rmdirArg.extraFlags & RmDirArg::THROW_ENOENT)) {
			throw std::system_error(errno, std::system_category(),
				"Failed to rmdir " + rmdirArg.pathname);
		}
	} else if ((rmdirArg.extraFlags & RmDirArg::DO_ACCOUNTING) &&
		   rmdirArg.fsIoStat != nullptr) {
		const auto vUs = static_cast<uint64_t>((vTpEnd - vTpStart) / 1us);
		rmdirArg.fsIoStat->IncOperations(FsOpId::RMDIR);
		rmdirArg.fsIoStat->AddMeasure(FsOpId::RMDIR, vUs);
	}
}

int CreateFile(const std::string &filepath, int flags, mode_t mode)
{
	int vFd = open(filepath.c_str(), OpenFlags::OF_CREAT | flags, mode);

	if (vFd < 0) {
		throw std::system_error(errno, std::system_category(),
			"Failed to create " + filepath);
	}

	return vFd;
}

int CreateFile(CreateFileArg &createArg)
{
	using namespace std::literals;

	PerfTimePoint vTpStart, vTpEnd;
	int vFd;

	if (createArg.extraFlags & CreateFileArg::IGNORE_EEXIST) {
		createArg.openFlags &= ~OpenFlags::OF_EXCL;
	}

	if (createArg.extraFlags & CreateFileArg::DO_ACCOUNTING) {
		vTpStart = PerfClock::now();
		vFd = openat(createArg.parentDirFd,
			createArg.pathname.c_str(),
			createArg.openFlags,
			createArg.mode);
		vTpEnd = PerfClock::now();
	} else {
		vFd = openat(createArg.parentDirFd,
			createArg.pathname.c_str(),
			createArg.openFlags,
			createArg.mode);
	}

	/* -1 vs. vFd(>0) */
	if (vFd < 0) {
		throw std::system_error(errno, std::system_category(),
			"Failed to create " + createArg.pathname);
	} else if ((createArg.extraFlags & CreateFileArg::DO_ACCOUNTING) &&
		   createArg.fsIoStat != nullptr) {
		const auto vUs = static_cast<uint64_t>((vTpEnd - vTpStart) / 1us);
		createArg.fsIoStat->IncOperations(FsOpId::CREATE);
		createArg.fsIoStat->AddMeasure(FsOpId::CREATE, vUs);
	}

	return vFd;
}

int OpenPath(const std::string &pathname, int flags)
{
	int vFd = open(pathname.c_str(), flags);

	if (vFd < 0) {
		throw std::system_error(errno, std::system_category(),
			"Failed to open " + pathname);
	}

	return vFd;
}

int OpenPath(struct OpenPathArg &openArg)
{
	using namespace std::literals;

	PerfTimePoint vTpStart, vTpEnd;
	int vFd;

	openArg.openFlags &= ~OpenFlags::OF_CREAT;

	if (openArg.extraFlags & OpenPathArg::DO_ACCOUNTING) {
		vTpStart = PerfClock::now();
		vFd = openat(openArg.parentDirFd,
			openArg.pathname.c_str(),
			openArg.openFlags);
		vTpEnd = PerfClock::now();
	} else {
		vFd = openat(openArg.parentDirFd,
			openArg.pathname.c_str(),
			openArg.openFlags);
	}

	/* -1 vs. vFd(>0) */
	if (vFd < 0) {
		throw std::system_error(errno, std::system_category(),
			"Failed to open" + openArg.pathname);
	} else if ((openArg.extraFlags & OpenPathArg::DO_ACCOUNTING) &&
		   openArg.fsIoStat != nullptr) {
		const auto vUs = static_cast<uint64_t>((vTpEnd - vTpStart) / 1us);
		openArg.fsIoStat->IncOperations(FsOpId::CREATE);
		openArg.fsIoStat->AddMeasure(FsOpId::CREATE, vUs);
	}

	return vFd;
}

void LinkPath(LinkPathArg &linkArg)
{
	using namespace std::literals;

	PerfTimePoint vTpStart, vTpEnd;
	int vRc;

	if (linkArg.extraFlags & LinkPathArg::DO_ACCOUNTING) {
		vTpStart = PerfClock::now();
		vRc = linkat(linkArg.srcParentFd,
			linkArg.srcPathname.c_str(),
			linkArg.dstParentFd,
			linkArg.dstPathname.c_str(),
			linkArg.flags);
		vTpEnd = PerfClock::now();
	} else {
		vRc = linkat(linkArg.srcParentFd,
			linkArg.srcPathname.c_str(),
			linkArg.dstParentFd,
			linkArg.dstPathname.c_str(),
			linkArg.flags);
	}

	/* 0 vs. -1 */
	if (vRc != 0) {
		throw std::system_error(errno, std::system_category(),
			"Failed to link " + linkArg.srcPathname + " to " + linkArg.dstPathname);
	} else if ((linkArg.extraFlags & LinkPathArg::DO_ACCOUNTING) &&
		   linkArg.fsIoStat != nullptr) {
		const auto vUs = static_cast<uint64_t>((vTpEnd - vTpStart) / 1us);
		linkArg.fsIoStat->IncOperations(FsOpId::LINK);
		linkArg.fsIoStat->AddMeasure(FsOpId::LINK, vUs);
	}
}

void UnlinkPath(const std::string &filepath)
{
	int vRc = unlink(filepath.c_str());

	if (vRc < 0) {
		throw std::string("Failed to unlink ") + filepath + " err: " + strerror(errno);
	}
}

void UnlinkPath(struct UnlinkPathArg &unlinkArg)
{
	using namespace std::literals;

	PerfTimePoint vTpStart, vTpEnd;
	int vRc;

	if (unlinkArg.extraFlags & UnlinkPathArg::DO_ACCOUNTING) {
		vTpStart = PerfClock::now();
		vRc = unlink(unlinkArg.pathname.c_str());
		vTpEnd = PerfClock::now();
	} else {
		vRc = unlink(unlinkArg.pathname.c_str());
	}

	/* 0 vs. -1 */
	if (vRc != 0) {
		throw std::system_error(errno, std::system_category(),
			"Failed to unlink " + unlinkArg.pathname);
	} else if ((unlinkArg.extraFlags & UnlinkPathArg::DO_ACCOUNTING) &&
		   unlinkArg.fsIoStat != nullptr) {
		const auto vUs = static_cast<uint64_t>((vTpEnd - vTpStart) / 1us);
		unlinkArg.fsIoStat->IncOperations(FsOpId::UNLINK);
		unlinkArg.fsIoStat->AddMeasure(FsOpId::UNLINK, vUs);
	}
}

void CloseFd(int fd)
{
	int vRc = close(fd);

	if (vRc < 0) {
		throw std::string("Failed to clase fd-") +
			std::to_string(fd) + " err: " + strerror(errno);
	}
}

void CloseFd(CloseFdArg &closeArg)
{
	using namespace std::literals;

	PerfTimePoint vTpStart, vTpEnd;
	int vRc;

	if (closeArg.extraFlags & CloseFdArg::DO_ACCOUNTING) {
		vTpStart = PerfClock::now();
		vRc = close(closeArg.fd);
		vTpEnd = PerfClock::now();
	} else {
		vRc = close(closeArg.fd);
	}

	/* 0 vs. -1 */
	if (vRc != 0) {
		throw std::system_error(errno, std::system_category(),
			"Failed to close fd-" + closeArg.fd);
	} else if ((closeArg.extraFlags & CloseFdArg::DO_ACCOUNTING) &&
		   closeArg.fsIoStat != nullptr) {
		const auto vUs = static_cast<uint64_t>((vTpEnd - vTpStart) / 1us);
		closeArg.fsIoStat->IncOperations(FsOpId::CLOSE);
		closeArg.fsIoStat->AddMeasure(FsOpId::CLOSE, vUs);
	}
}

void StatFd(int fd, struct stat *pStat)
{
	int vRc = fstat(fd, pStat);

	if (vRc < 0) {
		throw std::string("Failed to stat fd-") +
			std::to_string(fd) + " err: " + strerror(errno);
	}
}

void WriteFile(int fd, const char *buf, size_t len)
{
	ssize_t vRc = write(fd, buf, len);
	if (vRc == len) {
		return;
	} else if (vRc == -1) {
		throw std::string("FileSystem API exception triggered by WriteFile: ") + strerror(errno);
	} else if (vRc != len)  {
		throw std::string("FileSystem API exception triggered by WriteFile: ") + std::to_string(vRc);
	}
}

void ReadFile(int fd, char *buf, size_t len)
{
	ssize_t vRc = read(fd, buf, len);
	if (vRc == len || vRc == 0) {
		return;
	} else if (vRc == -1) {
		throw std::string("FileSystem API exception triggered by ReadFile: ") + strerror(errno);
	} else if (vRc != len) {
		throw std::string("FileSystem API exception triggered by ReadFile: ") + std::to_string(vRc);
	}
}

void SendFile(int destFd, int srcFd, off_t *pOffset, size_t bytes)
{
	ssize_t vRc = sendfile(destFd, srcFd, pOffset, bytes);

	if (vRc == bytes) {
		return;
	} else if (vRc == -1)  {
		throw std::string("FileSystem API exception triggered by SendFile: ") + strerror(errno);
	} else if (vRc != bytes) {
		throw std::string("FileSystem API exception triggered by SendFile: ") + std::to_string(vRc);
	}
}

void *MemMap(MemMapArg &mapArg)
{
	void *va = mmap(mapArg.virtualAddrHint, mapArg.len, mapArg.prot, mapArg.flags, mapArg.fd, mapArg.off);
	if (va != static_cast<void *>(MAP_FAILED)) {
		return va;
	}
	throw std::string("FileSystem API exception triggered by MemMap: ") + strerror(errno);
}

void MemUnmap(MemUnmapArg &unmapArg)
{
	int vRc = munmap(unmapArg.virtualAddr, unmapArg.len);
	if (vRc != 0) {
		throw std::string("FileSystem API exception triggered by MemUnmap: ") + strerror(errno);
	}
}

void MemSync(MemSyncArg &syncArg)
{
	int vRc = msync(syncArg.mapAddr, syncArg.mapLen, syncArg.flags);
	if (vRc != 0) {
		throw std::string("FileSystem API exception triggered by MemSync: ") + strerror(errno);
	}
}

void MemLock(MemLockArg &lockArg)
{
#if defined(GLIBC_HAS_MLOCK2)
#pragma message "The glibc has mlock2, use mlock2()."
	int vRc = mlock2(lockArg.virtualAddr, lockArg.len, lockArg.flags);
#else
#warning "The glibc has no mlock2, use mlock()!"
	int vRc = mlock(lockArg.virtualAddr, lockArg.len);
#endif
	if (vRc != 0) {
		throw std::string("FileSystem API exception triggered by MemLock: ") + strerror(errno);
	}
}

void MemUnlock(MemUnlockArg &unlockArg)
{
	int vRc = munlock(unlockArg.virtualAddr, unlockArg.len);
	if (vRc != 0) {
		throw std::string("FileSystem API exception triggered by MemUnlock: ") + strerror(errno);
	}
}

void Rename(RenameArg &renameArg)
{
	using namespace std::literals;

	const auto vTpStart = PerfClock::now();
#if defined(GLIBC_HAS_RENAMEAT2)
#pragma message "The glibc has renameat2(), use renameat2()."
	int vRc = renameat2(renameArg.oldParentDirFd, renameArg.oldPathname.c_str(),
			            renameArg.newParentDirFd, renameArg.newPathname.c_str(),
						renameArg.flags);
#else
#warning "The glibc has no renameat2(), use renameat()!"
	int vRc = renameat(renameArg.oldParentDirFd, renameArg.oldPathname.c_str(),
			            renameArg.newParentDirFd, renameArg.newPathname.c_str());
#endif
	const auto vTpEnd = PerfClock::now();
	const auto vUs = static_cast<uint64_t>((vTpEnd - vTpStart).count() * 1000000ul);

	if (vRc != 0) {
		throw std::system_error(errno, std::system_category(), "FileSystem API exception triggered by Rename");
	} else if (renameArg.fsIoStat != nullptr) {
		const auto vUs = static_cast<uint64_t>((vTpEnd - vTpStart) / 1us);
		renameArg.fsIoStat->IncOperations(FsOpId::RENAME);
		renameArg.fsIoStat->AddMeasure(FsOpId::RENAME, vUs);
	}
}

}  // namespace filesystem
}  // namespace liuzan

