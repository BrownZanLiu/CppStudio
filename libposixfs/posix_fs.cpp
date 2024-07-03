#include <chrono>  // std::chrono::steady_clock
#include <system_error>

#include <file_system.h>

namespace liuzan {
namespace filesystem {

FsIOStatics gFsIoStatics;

void MakeDir(const std::string &dirpath, mode_t mkFlags)
{
	int vRc = mkdir(dirpath.c_str(), mkFlags);

	if (vRc != 0 && errno != EEXIST) {
		throw std::string("FileSystem API exception triggered by MakeDir: ") + strerror(errno);
	}
}

int CreateFile(const std::string &filepath, int openFlags, mode_t createFlags)
{
	int fd = open(filepath.c_str(), O_CREAT | openFlags, createFlags);

	if (fd < 0) {
		throw std::string("FileSystem API exception triggered by CreateFile: ") + strerror(errno);
	}

	return fd;
}

int OpenPath(const std::string &filepath, int openFlags)
{
	int fd = open(filepath.c_str(), openFlags);

	if (fd < 0) {
		throw std::string("FileSystem API exception triggered by OpenPath: ") + strerror(errno);
	}

	return fd;
}

void CloseFd(int fd)
{
	int vRc = close(fd);

	if (vRc < 0) {
		throw std::string("FileSystem API exception triggered by CloseFd: ") + strerror(errno);
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

void StatFd(int fd, struct stat *pStat)
{
	int vRc = fstat(fd, pStat);

	if (vRc < 0) {
		throw std::string("FileSystem API exception triggered by StatFd: ") + strerror(errno);
	}
}

void UnlinkPath(const std::string &filepath)
{
	int vRc = unlink(filepath.c_str());

	if (vRc < 0) {
		throw std::string("FileSystem API exception triggered by UnlinkFile: ") + strerror(errno);
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
	int vRc = mlock2(lockArg.virtualAddr, lockArg.len, lockArg.flags);
#else
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

void MkDir(MkDirArg &mkdirArg)
{
	using namespace std::literals;

	const auto vTpStart = std::chrono::steady_clock::now();
	int vRc = mkdirat(mkdirArg.parentDirFd, mkdirArg.pathname.c_str(), mkdirArg.mode);
	const auto vTpEnd = std::chrono::steady_clock::now();

	if (vRc != 0 && (errno != EEXIST || !(mkdirArg.flags & MkDirArg::IGNORE_EEXIST))) {
		throw std::system_error(errno, std::system_category(), "FileSystem API exception triggered by MkDir");
	} else if (mkdirArg.fsIoStat != nullptr) {
		const auto vUs = static_cast<uint64_t>((vTpEnd - vTpStart) / 1us);
		mkdirArg.fsIoStat->IncOperations(filesystem_operation_id::MKDIR);
		mkdirArg.fsIoStat->AddMeasure(filesystem_operation_id::MKDIR, vUs);
	}
}

void RmDir(RmDirArg &rmdirArg)
{
	using namespace std::literals;

	const auto vTpStart = std::chrono::steady_clock::now();
	int vRc = rmdir(rmdirArg.pathname.c_str());
	const auto vTpEnd = std::chrono::steady_clock::now();

	if (vRc != 0) {
		throw std::system_error(errno, std::system_category(), "FileSystem API exception triggered by RmDir");
	} else if (rmdirArg.fsIoStat != nullptr) {
		const auto vUs = static_cast<uint64_t>((vTpEnd - vTpStart) / 1us);
		rmdirArg.fsIoStat->IncOperations(filesystem_operation_id::RMDIR);
		rmdirArg.fsIoStat->AddMeasure(filesystem_operation_id::RMDIR, vUs);
	}
}

void Rename(RenameArg &renameArg)
{
	using namespace std::literals;

	const auto vTpStart = std::chrono::steady_clock::now();
#if defined(GLIBC_HAS_RENAMEAT2)
	int vRc = renameat2(renameArg.oldParentDirFd, renameArg.oldPathname.c_str(),
			            renameArg.newParentDirFd, renameArg.newPathname.c_str(),
						renameArg.flags);
#else
	int vRc = renameat(renameArg.oldParentDirFd, renameArg.oldPathname.c_str(),
			            renameArg.newParentDirFd, renameArg.newPathname.c_str());
#endif
	const auto vTpEnd = std::chrono::steady_clock::now();
	const auto vUs = static_cast<uint64_t>((vTpEnd - vTpStart).count() * 1000000ul);

	if (vRc != 0) {
		throw std::system_error(errno, std::system_category(), "FileSystem API exception triggered by Rename");
	} else if (renameArg.fsIoStat != nullptr) {
		const auto vUs = static_cast<uint64_t>((vTpEnd - vTpStart) / 1us);
		renameArg.fsIoStat->IncOperations(filesystem_operation_id::RENAME);
		renameArg.fsIoStat->AddMeasure(filesystem_operation_id::RENAME, vUs);
	}
}

}  // namespace filesystem
}  // namespace liuzan

