#ifndef LIUZAN_FILE_SYSTEM_H

#include <cstdint>  // uint32_t, uint64_t
#include <string>  // std::string

#include <errno.h>
#include <fcntl.h>  // open(), AT_*
#include <stdio.h>  // rename*()
#include <string.h>  // strerror()
#include <sys/mman.h>  // mmap(), munmap()
#include <sys/sendfile.h>  // sendfile()
#include <sys/stat.h>  // mkdir(), fstat(), mode_t
#include <unistd.h>  // close(), rmdir()

#include <perf.h>  // filesystem_operation_id/FsIoStatistics

namespace liuzan {
namespace filesystem {

constexpr uint32_t BYTES_PER_PAGE = 4096u;
constexpr uint32_t QWORD_PER_PAGE = BYTES_PER_PAGE / 8u;
constexpr uint32_t BYTES_PER_MEGA = 1024u * 1024u;
constexpr uint32_t PAGES_PER_MEGA = BYTES_PER_MEGA / BYTES_PER_PAGE;
constexpr uint32_t BYTES_PER_GIGA = 1024u * 1024u * 1024u;
constexpr uint32_t PAGES_PER_GIGA = BYTES_PER_GIGA / BYTES_PER_PAGE;

extern FsIoStatistics gFsIoStatistics;

enum OpenFlags {
	// include/uapi/asm-generic/fcntl.h
	OF_CLOEXEC   = 02000000,
	OF_NOATIME   = 01000000,
	OF_NOFOLLOW  = 00400000,
	OF_DIRECTORY = 00200000,
	OF_LARGEFILE = 00100000,
	OF_DIRECT    = 00040000,
	OF_NONBLOCK  = 00004000,
	OF_APPEND    = 00002000,
	OF_TRUNC     = 00001000,
	OF_NOCTTY    = 00000400,
	OF_EXCL      = 00000200,
	OF_CREAT     = 00000100,
	OF_ACCMODE   = 00000003,
	OF_RDWR      = 00000002,
	OF_WRONLY    = 00000001,
	OF_RDONLY    = 00000000
};

enum CreateMode {
	// include/uapi/linux/stat.h
	CM_IFMT   = 0170000,
	CM_IFSOCK = 0140000,
	CM_IFLNK  = 0120000,
	CM_IFREG  = 0100000,
	CM_IFBLK  = 0060000,
	CM_IFDIR  = 0040000,
	CM_IFCHR  = 0020000,
	CM_IFIFO  = 0010000,

	CM_ISUID = 04000,
	CM_ISGID = 02000,
	CM_ISVTX = 01000,

	CM_IRWXU = 0700,
	CM_IRUSR = 0400,
	CM_IWUSR = 0200,
	CM_IXUSR = 0100,

	CM_IRWXG = 0070,
	CM_IRGRP = 0040,
	CM_IWGRP = 0020,
	CM_IXGRP = 0010,

	CM_IRWXO = 0007,
	CM_IROTH = 0004,
	CM_IWOTH = 0002,
	CM_IXOTH = 0001
};

void MkDir(const std::string &dirpath, mode_t mkFlags);
struct MkDirArg {
	explicit MkDirArg(const std::string &_pathname,
			int _parentDirFd = AT_FDCWD,
			mode_t _mode = CreateMode::CM_IRWXU | CreateMode::CM_IRWXG,
			FsIoStatistics *_fsIoStat = &gFsIoStatistics)
		: parentDirFd(_parentDirFd),
		pathname(_pathname),
		mode(_mode),
		fsIoStat(_fsIoStat),
		extraFlags(DO_ACCOUNTING)
	{
	}

	FsIoStatistics *fsIoStat;
	std::string pathname;
	int parentDirFd;
	mode_t mode;
	/**
	 * Without ACL enabled, effective mode >= (mode & 0777 & ~process_umask)
	 * See 'enum CreateMode' above.
	 */

	int extraFlags;
	enum mkdir_arg_flags {
		NONE = 0,
		DO_ACCOUNTING = 1 << 0,
		IGNORE_EEXIST = 1 << 1,
	};
};
void MkDir(MkDirArg &mkdirArg);

struct RmDirArg {
	explicit RmDirArg(const std::string &_pathname,
		FsIoStatistics *_fsIoStat = &gFsIoStatistics)
		: pathname(_pathname),
		fsIoStat(_fsIoStat),
		extraFlags(DO_ACCOUNTING)
	{}

	std::string pathname;
	FsIoStatistics *fsIoStat;

	int extraFlags;
	enum rmdir_arg_flags {
		NONE = 0,
		DO_ACCOUNTING = 1 << 0,
	};
};
void RmDir(RmDirArg &rmdirArg);

int CreateFile(const std::string &filepath, int openFlags, mode_t createFlags);
struct CreateFileArg {
	explicit CreateFileArg(std::string _pathname,
		int _parentDirFd = AT_FDCWD,
		int _openFlags = OpenFlags::OF_CREAT | OpenFlags::OF_EXCL | OpenFlags::OF_RDWR,
		mode_t _mode = CreateMode::CM_IFREG | CreateMode::CM_IRUSR | CreateMode::CM_IWUSR |
			CreateMode::CM_IRGRP | CreateMode::CM_IROTH,
		FsIoStatistics *_fsIoStat = &gFsIoStatistics)
		: fsIoStat(_fsIoStat),
		pathname(_pathname),
		parentDirFd(_parentDirFd),
		openFlags(_openFlags),
		mode(_mode),
		extraFlags(DO_ACCOUNTING)
	{
	}

	FsIoStatistics *fsIoStat;
	std::string pathname;
	int parentDirFd;
	int openFlags;
	mode_t mode;
	/**
	 * If and only if O_CREAT or O_TMPFILE specified in openFlags,
	 * mode will be and must be considered.
	 * MUST means not to ignore it. Otherwise, there would be undefined behavior.
	 * Supported values, see 'enum class CreateMode' above.
	 */

	int extraFlags;
	enum createfile_arg_flags {
		NONE = 0,
		DO_ACCOUNTING = 1 << 0,
		IGNORE_EEXIST = 1 << 1,
	};
};
int CreateFile(struct CreateFileArg &createFileArg);

int OpenPath(const std::string &filepath, int openFlags);

enum LinkFlags {
	// include/uapi/linux/fcntl.h
	LF_SYMLINK_FOLLOW = 0x400,
	/**
	 * If set, the link operation will dereferrence the srcPathname if it's symbolic link.
	 * Then, the target will link to the backing file instead of the symbolic file itself.
	 */

	LF_EMPTY_PATH = 0x1000
	/**
	 * With AT_EMPTY_PATH, the srcPathname could be empty and the srcParentFd (may have been
	 * obtained using the open O_PATH flag) would be used as the srouce file (MUST NOT be dir).
	 * Caller must have the CAP_DAC_READ_SEARCH capability.
	 */
};

struct LinkPathArg {
	explicit LinkPathArg(std::string _srcPathname, std::string _dstPathname,
		int _srcParentFd = AT_FDCWD,
		int _dstParentFd = AT_FDCWD,
		int _flags = 0,
		FsIoStatistics *_fsIoStat = &gFsIoStatistics)
		: fsIoStat(_fsIoStat),
		srcParentFd(_srcParentFd),
		dstParentFd(_dstParentFd),
		srcPathname(_srcPathname),
		dstPathname(_dstPathname),
		flags(_flags),
		extraFlags(DO_ACCOUNTING)
	{
	}

	FsIoStatistics *fsIoStat;
	int srcParentFd;
	int dstParentFd;
	std::string srcPathname;
	std::string dstPathname;
	int flags;

	int extraFlags;
	enum link_arg_flags {
		NONE = 0,
		DO_ACCOUNTING = 1 << 0,
	};
};
void LinkPath(LinkPathArg &linkArg);

void UnlinkPath(const std::string &filepath);
struct UnlinkPathArg {
	explicit UnlinkPathArg(std::string _pathname, FsIoStatistics *_fsIoStat = &gFsIoStatistics)
		: fsIoStat(_fsIoStat),
		pathname(_pathname),
		extraFlags(DO_ACCOUNTING)
	{
	}

	FsIoStatistics *fsIoStat;
	std::string pathname;

	int extraFlags;
	enum unlink_arg_flags {
		NONE = 0,
		DO_ACCOUNTING = 1 << 0,
	};
};
void UnlinkPath(UnlinkPathArg &unlinkArg);

void CloseFd(int fd);
struct CloseFdArg {
	explicit CloseFdArg(int _fd, FsIoStatistics *_fsIoStat = &gFsIoStatistics)
		: fsIoStat(_fsIoStat),
		fd(_fd),
		extraFlags(DO_ACCOUNTING)
	{
	}

	FsIoStatistics *fsIoStat;
	int fd;

	int extraFlags;
	enum close_arg_flags {
		NONE = 0,
		DO_ACCOUNTING = 1 << 0,
	};
};
void CloseFd(CloseFdArg &closeArg);

void StatFd(int fd, struct stat *pStat);

/**
 * It's a surprise if no expected bytes are read or writtern.
 * Rationale:
 * 1) It's prefered to make the main logic simple.
 * 2) Let the upper layer to handle surprise based on their scenarioes.
 * 3) Optimize when we really need.
 */
void WriteFile(int fd, const char *buf, size_t len);

void ReadFile(int fd, char *buf, size_t len);

void SendFile(int destFd, int srcFd, off_t *pOffset, size_t bytes);

struct MemMapArg {
	MemMapArg(int _fd, int _flags, int _prot, size_t _len, off_t _off = 0, void *_vaHint = nullptr)
		: fd(_fd), flags(_flags), prot(_prot), len(_len), off(_off), virtualAddrHint(_vaHint)
	{}

	void *virtualAddrHint;
	/**
	 * If it's nullptr, OS will choose a page-aligned virtual address.
	 */
	int fd;
	int flags;
	/**
	 * Exclusive flags for sharing or writeback:
	 *     -MAP_SHARED: Updates visible to other process and will be written back to underlying file if has one; use
	 *     msync() to write back immediately. POSIX.
	 *	   -MAP_SHARED_VALIDATE: Same as MAP_SHARED except that it will fail out with EOPNOTSUPP. Some flags need it to
	 *	   work. MAP_SYNC is such one. This is a linux extension.
	 *	   -MAP_PRIVATE: Create a copy-on-write mapping. Updates invisible to other process and won't be written back to
	 *	   underlying file even if hase one. And, file updates might invisible after mapping done. POSIX.
	 * Other flags could be Ored to above:
	 *     -MAP_ANONYMOUS|MAP_ANON: No file-backed mapping. Initialized to zero. Offset must be zero. fd should be -1
	 *     for best compatibility. Supported by most other UNIX-like systems.
	 *     -MAP_FIXED: Consider 'virtualAddrHint' as a forcement. Page alignment is still required. Existing mapping[s]
	 *     will be discarded if possible. mmap() will fail if alignment exception, discard failure, invalid addr, etc.
	 *     POSIX.
	 *     -MAP_FIXED_NOREPLACE: Same as MAP_FIXED except that will fail out with EEXIST instead of discarding existing
	 *     mapping[s]. In a process, only one thread could atomically map the given region with other threads. Before
	 *     Linux 4.17, it might fall back to non-MAP_FIXED version. So, check the return address for portability.
	 *     -MAP_GROWSDOWN: Used for stack. Return an address one-page-lower than the actually allocated process virtual
	 *     address space. The return address is the starting address of a 'guard' page. When any address in this guard
	 *     page is touched, the mmaping is extended downward. If the extension encounter an existing mapping, SIGSEGV
	 *     will be triggered.
	 *     -MAP_HUGETLB: Allocate the mapping using 'huge' pages.
	 *     -MAP_HUGE_2MB|MAP_HUGE_1GB: Used with MAP_HUGETLB to select hugepage size.
	 *     -MAP_LOCKED: Mark the mapped region to be locked in the same way as mlock(). Try to populate mapping region
	 *     as mlock(). However, mmap won't fail with ENOMEM as mlock(). So, it's possible to experience major fault
	 *     after mmap(). If we expect all done as expected or fail, use mmap() plus mlock().
	 *     -MAP_NONBLOCK: Just meaningful with MAP_POPULATE. Now, it'll make MAP_POPULATE to be ignored. Don't
	 *     read-ahead and create page tables entries only for pages that are already present in RAM.
	 *     -MAP_POPULATE: Populate (prefault) page tables for a mapping. For a file mapping, this causes read-ahead on
	 *     the file. This will help to reduce blocking on page faults later. The mmap() call doesn't fail if the mapping
	 *     cannot be populated.
	 *     -MAP_NORESERVE: Do not reserve swap space for this mapping. When swap space is not reserved one might get
	 *     SIGSEGV upon a write if no physical memory is available.
	 *     -MAP_SYNC: Only valid for DAX(direct mapping of persistent memory) file system. Must Ored with
	 *     MAP_SHARED_VALIDATE. EOPNOTSUPP reported by mmap() if used on non-DAX file.
	 *     -MAP_UNINITIALIZED: Don't clear anonymous pages. This flag is intended to improve performance on embedded
	 *     devices. This flag is honored only if the kernel was configured with the CONFIG_MMAP_ALLOW_UNINITIALIZED
	 *     option.
	 */
	off_t off;
	/**
	 * Should be page-aligned or hugepage-aligned.
	 */
	size_t len;
	int prot;
	/**
	 * Memory protection of mapping: PROT_EXEC | PROT_READ | PROT_WRITE | PROT_NONE
	 * Must not conflict with file open mode.
	 */

	/** ==== extention ====
	 * Statistics per file system?
	 */
};
void *MemMap(MemMapArg &mapArg);
/**
 * Creates a new mapping[vm_area_struct] in the virtual address space of the calling process.
 * Return MAP_FAILED(-1) if failed.
 * No file size modification will be synchronized to backing file even we have one.
 */

struct MemUnmapArg {
	MemUnmapArg(void *_va, size_t _len): virtualAddr(_va), len(_len) {}

	void *virtualAddr;  // Returned by MemMap().
	size_t len;
	/**
	 * In MAP_HUGETLB, it's needed to be hugepage-alined.
	 */

	/** ==== extention ====
	 * Statistics per file system?
	 */
};
void MemUnmap(MemUnmapArg &unmapArg);
/**
 * Make access on [virtualAddr, virtualAddr+len] to trigger SIGSEGV.
 * It's OK if no mapping on [virtualAddr, virtualAddr+len].
 * Process termination could also delete the target mapping if no unmapping done.
 * Closing file won't delete mapping[s].
 * This will guarantee dirty pages written back to file. See msync().
 */

struct MemSyncArg {
	MemSyncArg(void *_va, size_t _len, int _flags): mapAddr(_va), mapLen(_len), flags(_flags) {}

	void *mapAddr;
	size_t mapLen;
	int flags;
	/**
	 * Exclusive flags for flushing behavior:
	 *     -MS_ASYNC: Specifies that an update be scheduled, but the call returns immediately.
	 *     -MS_SYNC: Requests an update and waits for it to complete.
	 * Other flags could be Ored with above:
	 *     -MS_INVALIDATE: Asks to invalidate other mappings of the same file so that that they can be updated with the
	 *     fresh values just written.
	 */
};
void MemSync(MemSyncArg &syncArg);
/**
 * Flush changes to backing file.
 * Exception throw if failed.
 */

struct MemLockArg {
	MemLockArg(const void *_va, size_t _len, unsigned int _flags = 0)
		: virtualAddr(_va), len(_len), flags(_flags)
	{}

	const void *virtualAddr;
	size_t len;
	unsigned int flags;
	/**
	 * 0, fault-in all pages and lock them in memory to avoid swapping-out.
	 * MLOCK_ONFAULT, Lock pages that are currently resident and mark the entire range so that the
	 * remaining nonresident pages are locked when they are populated by a page fault.
	 */
};
void MemLock(MemLockArg &lockArg);

struct MemUnlockArg {
	MemUnlockArg(const void *_va, size_t _len): virtualAddr(_va), len(_len) {}

	const void *virtualAddr;
	size_t len;
};
void MemUnlock(MemUnlockArg &unlockArg);

struct RenameArg {
	RenameArg(int _oldParentFd, std::string _oldPathname,
			  int _newParentFd, std::string _newPathname,
			  unsigned int _flags = 0u,
			  FsIoStatistics *_fsIoStat = &gFsIoStatistics)
		: oldParentDirFd(_oldParentFd), oldPathname(_oldPathname),
		  newParentDirFd(_newParentFd), newPathname(_newPathname),
		  flags(_flags), fsIoStat(_fsIoStat)
	{}

	int oldParentDirFd;
	int newParentDirFd;
	std::string oldPathname;
	std::string newPathname;
	unsigned int flags;
	/**
	 * Only supported by renameat2().
	 * Falgs supported:
	 * -RENAME_EXCHANGE, atomically exchange oldPathname and newPathname.
	 * -RENAME_NOREPLACE, don't overwrite newPathname if it exists as usually do.
	 * -RENAME_WHITEOUT, only meaningful for overlay/union filesystem.
	 */
	FsIoStatistics *fsIoStat;
};
void Rename(RenameArg &renameArg);
/**
 * Rename a file system dentry or move an dentry into another location in the same file system tree.
 * If the new place has a dentry with the target name, try to delete it first.
 * It's impossible to move a dentry into its descendant directory.
 */

}  // namespace filesystem
}  // namespace liuzan

#endif  // LIUZAN_FILE_SYSTEM_H
