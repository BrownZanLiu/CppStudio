#ifndef LIUZAN_FILE_SYSTEM_H

#include <cstdint>  // uint32_t, uint64_t
#include <string>  // std::string

#include <errno.h>
#include <fcntl.h>  // open(), AT_*
#include <string.h>  // strerror()
#include <sys/mman.h>  // mmap(), munmap()
#include <sys/sendfile.h>  // sendfile()
#include <sys/stat.h>  // mkdir(), fstat(), mode_t
#include <unistd.h>  // close()

namespace liuzan {
namespace filesystem {

enum class open_flags {
	// include/uapi/asm-generic/fcntl.h
	OF_O_CLOEXEC   = 02000000,
	OF_O_NOATIME   = 01000000,
	OF_O_NOFOLLOW  = 00400000,
	OF_O_DIRECTORY = 00200000,
	OF_O_LARGEFILE = 00100000,
	OF_O_DIRECT    = 00040000,
	OF_O_NONBLOCK  = 00004000,
	OF_O_APPEND    = 00002000,
	OF_O_TRUNC     = 00001000,
	OF_O_NOCTTY    = 00000400,
	OF_O_EXCL      = 00000200,
	OF_O_CREAT     = 00000100,
	OF_O_ACCMODE   = 00000003,
	OF_O_RDWR      = 00000002,
	OF_O_WRONLY    = 00000001,
	OF_O_RDONLY    = 00000000
};

enum class create_mode {
	// include/uapi/linux/stat.h
	CM_S_IFMT   = 0170000,
	CM_S_IFSOCK = 0140000,
	CM_S_IFLNK  = 0120000,
	CM_S_IFREG  = 0100000,
	CM_S_IFBLK  = 0060000,
	CM_S_IFDIR  = 0040000,
	CM_S_IFCHR  = 0020000,
	CM_S_IFIFO  = 0010000,

	CM_S_ISUID = 04000,
	CM_S_ISGID = 02000,
	CM_S_ISVTX = 01000,

	CM_S_IRWXU = 0700,
	CM_S_IRUSR = 0400,
	CM_S_IWUSR = 0200,
	CM_S_IXUSR = 0100,

	CM_S_IRWXG = 0070,
	CM_S_IRGRP = 0040,
	CM_S_IWGRP = 0020,
	CM_S_IXGRP = 0010,

	CM_S_IRWXO = 0007,
	CM_S_IROTH = 0004,
	CM_S_IWOTH = 0002,
	CM_S_IXOTH = 0001
};

constexpr uint32_t BYTES_PER_PAGE = 4096u;
constexpr uint32_t QWORD_PER_PAGE = BYTES_PER_PAGE / 8u;
constexpr uint32_t BYTES_PER_MEGA = 1024u * 1024u;
constexpr uint32_t PAGES_PER_MEGA = BYTES_PER_MEGA / BYTES_PER_PAGE;
constexpr uint32_t BYTES_PER_GIGA = 1024u * 1024u * 1024u;
constexpr uint32_t PAGES_PER_GIGA = BYTES_PER_GIGA / BYTES_PER_PAGE;

/**** For surprise, all APIs throw std::string message. ****/
void MakeDir(const std::string &dirpath, mode_t mkFlags);

int CreateFile(const std::string &filepath, int openFlags, mode_t createFlags);

int OpenPath(const std::string &filepath, int openFlags);

void CloseFd(int fd);

void SendFile(int destFd, int srcFd, off_t *pOffset, size_t bytes);

void StatFd(int fd, struct stat *pStat);

void UnlinkPath(const std::string &filepath);

/**
 * It's a surprise if no expected bytes are read or writtern.
 * Rationale:
 * 1) It's prefered to make the main logic simple.
 * 2) Let the upper layer to handle surprise based on their scenarioes.
 * 3) Optimize when we really need.
 */
void WriteFile(int fd, const char *buf, size_t len);
void ReadFile(int fd, char *buf, size_t len);

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

}  // namespace filesystem
}  // namespace liuzan

#endif  // LIUZAN_FILE_SYSTEM_H
