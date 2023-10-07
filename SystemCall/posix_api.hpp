/**
 * Copyright: brown.liuzan@outlook.com
 * Description: C++ Abstraction of Posix API that focuses on Linux system calls.
 * Version: kernel-5.14
 * References:
 * 1) https://man7.org/linux/man-pages/man2/syscalls.2.html
 * 2) /usr/include/asm/unistd_64.h included by asm/unistd.h included by sys/syscall.h
 * 3) /usr/include/bits/syscall.h included by sys/syscall.h
 * 4) /usr/include/unistd.h
 * 5) /usr/include/features.h included by unistd.h
 * for _ISO*|_POSIX_*|_XOPEN_*|__USE_*|__GNU*|__GLIBC*|*_SOURCE
 *
 */

#ifndef LIUZAN_SYSTEMCALL_POSIX_API_HPP
#define LIUZAN_SYSTEMCALL_POSIX_API_HPP



#include <filesystem>
#include <system_error>  // std::error_code, std::errc, std::error_category, std::error_condition

#include <fcntl.h>  // Definitions of AT_* constants
#include <sys/syscall.h>  // system call numbers and SYS_* macroes for syscall()
#include <unistd.h>  // GLIBC



namespace liuzan {
namespace systemcall {
namespace posix {

enum class Accessibility {
	// It's legal to ORed them.
	F_OK = 0,  // file existence
	X_OK = 1,  // file existence and executablility
	W_OK = 2,  // file existence and writablility
	R_OK = 4,  // file existence and readablility
	RX_OK = 5,
	RW_OK = 6,
	RWX_OK = 7,
};

enum class AccessibilityCheckMode {
	/**
	 * __USE_ATFILE must be defined
	 * From: fcntl.h
	 * */

	AT_SYMLINK_NOFOLLOW = 0x100,
	/**
	 * Only used in faccessat() or faccessat2() to check accessibility with effective IDs instead
	 * of real IDs as default.
	 * As normal file operations done.
	 */
	AT_EACCESS = 0x200,
	AT_SYMLINK_FOLLOW = 0x400,
};

enum class AtFlags {
	/* __USE_ATFILE must be defined */

	/**
	 * Special value of dirfd of all *at() file system system calls.
	 * Indicate a retative pathname to the current working direcotry.
	 */
	AT_FDCWD = -100,
	AT_SYMLINK_NOFOLLOW = 0x100,
	/**
	 * Only used in unlinkat() to remove directory instead of unlinking file.
	 */
	AT_REMOVEDIR = 0x200,
	/**
	 * Only used in faccessat() or faccessat2() to check accessibility with effective IDs instead
	 * of real IDs as default.
	 * As normal file operations done.
	 */
	AT_EACCESS = 0x200,
	AT_SYMLINK_FOLLOW = 0x400,

	/* __USE_GNU must be defined */
	AT_NO_AUTOMOUNT = 0x800,
	AT_EMPTY_PATH = 0x1000,  // allow empty relative path
	AT_STATX_SYNC_AS_STAT = 0x0000,
	AT_STATX_FORCE_SYNC = 0x2000,
	AT_STATX_DONT_SYNC = 0x4000,
	AT_STATX_SYNC_TYPE = 0x6000,
	AT_RECURSIVE = 0x8000,  // apply to the whole subtree
};

enum class SeekOrigin {
	SEEK_SET = 0,  // Origin is the beginning of file. Seek before this origin is illegal.
	SEEK_CUR = 1,  // Origin is the current position. Both forward and backward might be legal.
	SEEK_END = 2,  // Origin is the end of file. Forward might create hole.
	/* Followings are on the way into POSIX.*/
	SEEK_DATA = 3,  // Jump to the next data region or ENXIO if offset resides in a hole. Otherwise, exacactly the offset.
	SEEK_HOLE = 4,  // Jump to the next hole or ENXIO if offset resides in a data region. Otherwise, exacactly the offset.
};

class GlibcSystemCall {
/**
 * Encapsulation of following fucntion:
 *     long syscall(long SYS_*, ...);
 *
 * References:
 *     1) <KernelSourceTreeRoot>/include/linux/syscalls.h: SYSCALL_DEFIINEx()
 *     2) /usr/include/asm/unistd_64.h: __NR_*
 *     3) /usr/include/bits/syscall.h: SYS_*
 *     4) /usr/include/unitstd.h: glibc wrapper functions
 *     5) /usr/include/fcntl.h: AT_*
 */
public:
	/**
	 * Linux system call semantics:
	 *     See FileSystem::IsPathAccessible()
	 *
	 * Linux system headers:  // __NR_faccessat2 439; Linux-specific to fix faccessat system call.
	 *     fcntl.h for AT_*
	 *     sys/syscall.h for SYS_*
	 *     unistd.h for syscall()
	 *
	 * Linux system call synopsis:
	 *     long (SYS_faccessat2, int dirfd, char const *pathname, int mode, int flags);
	 */
	static std::error_code IsPathAccessible(
			int fdParent,
			char const *pathname,
			Accessibility accessIntent,
			AccessibilityCheckMode checkMode);

	/**
	 * Linux system call semantics:
	 *     Close all file handles of given range.
	 *
	 * Headers:  // __NR_close_range 436
	 *     sys/syscall.h for SYS_*
	 *     unistd.h for syscall()
	 * Linux system clall synopsis:  // __USE_GNU
	 *     long syscall(SYS_close_range, unsigned fd, unsigned maxfd, unsigned flags);
	 */
	static std::error_code CloseRange(int fdStart, int fdEnd, unsigned flags);
};

class FileSystem {
/**
 * Some conventions on comment Linux file system concepts:
 * 1) File descriptor: It's a confusing name associated with the following
 * file description. It's a per-process integer identifier of an open file. It's used to index
 * kernel file description in the open file table.
 * 2) File description: It's a kernel object for description of an open file. It's of 'struct file'.
 * It records openning state such as current access position, current access mode and so on.
 * 3) File inode: It's a kernel object for description of a file no matter open or not. It's of
 * 'struct inode'. It records the globally identical state of a file.
 * 4) Usually, open() will create an file description object to access the target file inode and
 * identify it with file handle for usage by user space.
 * 5) Multiple opens on one file will create multiple file descriptions on the same file inode. And,
 * idenfied by multiple file handles.
 * 6) Usually one file handle per file description except that intentionally duplicate them.
 *
 * References:
 *     1) <KernelSourceTreeRoot>/include/linux/syscalls.h: SYSCALL_DEFIINEx()
 *     2) /usr/include/asm/unistd_64.h: __NR_*
 *     3) /usr/include/bits/syscall.h: SYS_*
 *     4) /usr/include/unitstd.h: glibc wrapper functions
 *     5) /usr/include/fcntl.h
 */

public:
	/**
	 * Linux system call semantics:
	 *
	 * Linux system call synopsis:  // <KernelRoot>/fs/open.c
	 *
	 * Linux system headers:
	 *
	 * Glibc synopsis:
	 *
	 * Notes on Linux implementation:
	 *
	 */

	/**
	 * Linux system call semantics:
	 *     Open and possibly create a file if the file doesn't exist and O_CREAT set in oflag.
	 *     During execve, file operators won't be closed except if O_CLOEXEC set in oflag.
	 *     Return a new file descriptor for subsequent usages on read/write/lseek/fcntl/... for efficiency. The kernel
	 *     try to choose a smallest unused integer as the new file descriptor. A file descriptor is a reference to a
	 *     file description which records file-accessing status such as offset, readahead, address_space loaded,
	 *     writeback error, syncfs error and so on. This reference won't be effected by unlinking/renaming the pathname.
	 *     The file offset is set the beginnning.
	 *     See struct files_struct, struct fdtable, struct file, struct fd, etc.
	 *
	 *
	 * Linux system call synopsis:
	 *     // __NR_open 2; <KernelRoot>/fs/open.c
	 *     long sys_open(char const *pathname, int flags, umode_t mode);
	 *     // __NR_creat 85; <KernelRoot>/fs/open.c
	 *     long sys_creat(char const *pathname, umode_t mode);
	 *     // __NR_openat 257; <KernelRoot>/fs/open.c
	 *     long sys_openat(int dfd, char const *pathname, int flags, umode_t mode);
	 *     // __NR_openat2 437; <KernelRoot>/fs/open.c; kernel-5.6
	 *     long sys_openat2(int dfd, char const *pathname, struct open_how *how, size_t usize);
	 *
	 * Linux system headers:
	 *     fcntl.h
	 *     // __OPEN_NEEDS_MODE(oflag:O_CREAT|__O_TMPFILE), mode_t, off_t, off64_t, pid_t
	 *     // S_IF*, S_IS*, S_I*USR, S_IRWXU, S_I*GRP, S_IRWXG, S_I*OTH, S_IRWXO
	 *     // F_OK, X_OK, W_OK, R_OK
	 *     // SEEK_SET|CUR|END
	 *     // AT_*
	 *          features.h
	 *          bits/types.h  // __mode_t, __dev_t, __off_t, __off64_t and so on
	 *          bits/fcntl.h  // O_*, F_*, FD_*; all the numbers and flag bits for open, fcntl and so on
	 *          bits/types/struct_timespec.h
	 *          bits/stat.h  // __S_IF*, __S_IS*, __S_I*
	 *
	 * Glibc synopsis:
	 *     int open(char const *file, int oflag, ...);
	 *     int openat(int dfd, char const *pathname, int oflag, ...);
	 *     int creat(char const *filename, mode_t mode);
	 *     // __USE_LARGEFILE64 || __USE_FILE_OFFSET64
	 *     int openat64(int dfd, char const *file, int oflag, ...);
	 *     int open64(char const *file, int oflag, ...);
	 *     int creat64(char const *filename, mode_t mode);
	 *
	 * Notes on Linux implementation:
	 *     They are cancellation points.
	 *
	 */

	/**
	 * Linux system call semantics:
	 *
	 * Linux system call synopsis:
	 *     // __NR_name_to_handle_at 303; <KernelRoot>/fs/fhandle.c
	 *     long name_to_handle_at(int dfd, char const *name, struct file_handle *handle, int *mnt_id, int flags);
	 *     // __NR_open_by_handle_at 304; <KernelRoot>/fs/fhandle.c
	 *     long sys_open_by_handle_at(int mountdirfd, struct file_handle *handle, int flags);
	 *
	 * Linux system headers:
	 *
	 * Glibc synopsis:
	 *
	 * Notes on Linux implementation:
	 *
	 */

	/**
	 * Linux system call semantics:
	 *
	 * Linux system call synopsis:
	 *     // __NR_open_tree 428; <KernelRoot>/fs/namespace.c; kernel-5.2
	 *     long sys_open_tree(int dfd, char const *pathname, unsigned flags);
	 *     // __NR_fsopen 430; <KernelRoot>/fs/fsopen.c; kernel-5.2
	 *     long sys_fsopen(char const *fsname, unsigned flags);
	 *
	 * Linux system headers:
	 *
	 * Glibc synopsis:
	 *
	 * Notes on Linux implementation:
	 *
	 */

	/**
	 * Linux system call semantics:
	 *     See https://man7.org/linux/man-pages/man2/read.2.html
	 *
	 * Linux system call synopsis:  // <KernelRoot>/fs/read_write.c
	 *     // __NR_read 0
	 *     long sys_read(unsigned fd, char *buf, size_t count);
	 *     // __NR_pread64 17
	 *     // __NR_readv 19
	 *     // __NR_preadv 295
	 *     // __NR_preadv2 327
	 *
	 * Linux system headers:
	 *     unistd.h
	 *
	 * Glibc synopsis:
	 *     ssize_t read(int fd, void *buf, size_t count);
	 *
	 * Notes on Linux implementation:
	 *
	 */

	/**
	 * Linux system call semantics:
	 *
	 * Linux system call synopsis:  // <KernelRoot>/fs/read_write.c
	 *     // __NR_readahead 187
	 *
	 * Linux system headers:
	 *     unistd.h
	 *
	 * Glibc synopsis:
	 *
	 * Notes on Linux implementation:
	 *
	 */

	/**
	 * Linux system call semantics:
	 *
	 * Linux system call synopsis:
	 *     // __NR_readlink 89
	 *
	 * Linux system headers:
	 *
	 * Glibc synopsis:
	 *
	 * Notes on Linux implementation:
	 *
	 */

	/**
	 * Linux system call semantics:
	 *
	 * Linux system call synopsis:
	 *     // __NR_process_vm_readv 310
	 *
	 * Linux system headers:
	 *
	 * Glibc synopsis:
	 *
	 * Notes on Linux implementation:
	 *
	 */

	/**
	 * Linux system call semantics:
	 *     Check if the calling process has the given accessing permission or if file exists.
	 *     Symbolic link will be dereferenced.
	 *     Check is done with read UID/GID instead of effective IDs. The laters are used for
	 *     ordinary file system operations. This give the set-uid/gid program a chance to check if
	 *     its invoker has the right to access file system.
	 *     Besides, for root user, permitted capabilites will be used instead of effective
	 *     capabilities. And, for non-root user, empty capabilities is used.
	 *     All ancestors must be X_OK. Otherwise, inaccessible. Linux always honors X_OK even the
	 *     user is a super one and this conflicts with POSIX.1-2001.
	 *     No type and content is involved. For directory, writable means file creation and
	 *     deletion.
	 *     For access() and faccessat(AT_FDCWD,...), a relative pathname is relative to the current
	 *     directory.
	 *
	 *     On modern Unix/Linux, roo-squash might be supported by all kinds of underly file system.
	 *
	 *     It's better with security to witch to real user before invoking open() under set-uid/gid
	 *     environment.
	 *
	 * Linux system call synopsis:  // <KernelRoot>/fs/open.c
	 *     // __NR_access 21
	 *     long access(char const *pathname, int mode);
	 *     // __NR_faccessat 269
	 *     long faccessat(int fdParent, char const *pathname, int mode);
	 *     // __NR_faccessat2 439
	 *     long faccessat2(int fdParent, char const *pathname, int mode, int flags);
	 *
	 * Linux system headers:
	 *     unistd.h
	 * Glibc synopsis:
	 *     int access(const char *pathname, int mode);
	 * Glibc synopsis:  // __USE_GNU
	 *     int euidaccess(char const *pathname, int mode);
	 *     int eaccess(char const *pathname, int mode);
	 * Glibc synopsis:  // __USE_ATFILE
	 *     int faccessat(int dfd, char const *pathname, int mode, int flags);
	 *
	 * Notes on Linux implementation:
	 *     Linux system call faccessat has no support on flags.
	 *     Instead, from Linux 5.8 on, syscall(SYS_faccessat2, fd4Parent, pathname, mod, flags)
	 *     supports it.
	 *     Before Glibc2.33, implements it with faccessat and fstatat system calls and has bugs with
	 *     ACLs.
	 *     From Glibc 2.33 on, implements it with faccessat2 system call.
	 *     To check symbol link itself, use IsPathAccessible() with AT_SYMLINK_NOFOLLOW.
	 */
	static std::error_code IsPathAccessible(
			char const * const pathname,
			Accessibility accessIntent);
	static std::error_code IsPathExisted(char const *const pathname)
	{
		return IsPathAccessible(pathname, F_OK);
	}
	static std::error_code IsPathExecutable(char const *const pathname)
	{
		return IsPathAccessible(pathname, X_OK);
	}
	static std::error_code IsPathWritable(char const *const pathname)
	{
		return IsPathAccessible(pathname, W_OK);
	}
	static std::error_code IsPathReadable(char const *const pathname)
	{
		return IsPathAccessible(pathname, R_OK);
	}
	static std::error_code IsPathReadableAndExecutable(char const *const pathname)
	{
		return IsPathAccessible(pathname, RX_OK);
	}
	static std::error_code IsPathReadableAndWritable(char const *const pathname)
	{
		return IsPathAccessible(pathname, RW_OK);
	}
	static std::error_code IsPathFullyAccessible(char const *const pathname)
	{
		return IsPathAccessible(pathname, RWX_OK);
	}

	static std::error_code IsPathAccessible(
			int fdParent,
			char const *const pathname,
			Accessibility accessIntent,
			AccessibilityCheckMode checkMode)
	{
		return GlibcDirectSystemCall::IsPathAccessible(fdParent, pathname, accessIntent, checkMode);
	}
	static std::error_code IsPathExisted4EID(char const *pathname)
	{
		return IsPathAccessible(
				AtFlags::AT_FDCWD, pathname, Accessibility::F_OK, AccessibilityCheckMode::AT_EACCESS);
	}
	static std::error_code IsPathExecutable4EID(char const *pathname)
	{
		return IsPathAccessible(
				AtFlags::AT_FDCWD, pathname, Accessibility::X_OK, AccessibilityCheckMode::AT_EACCESS);
	}
	static std::error_code IsPathWritable4EID(char const *pathname)
	{
		return IsPathAccessible(
				AtFlags::AT_FDCWD, pathname, Accessibility::W_OK, AccessibilityCheckMode::AT_EACCESS);
	}
	static std::error_code IsPathReadable4EID(char const *pathname)
	{
		return IsPathAccessible(
				AtFlags::AT_FDCWD, pathname, Accessibility::R_OK, AccessibilityCheckMode::AT_EACCESS);
	}
	static std::error_code IsPathReadableAndExecutable4EID(char const *pathname)
	{
		return IsPathAccessible(
				AtFlags::AT_FDCWD, pathname, Accessibility::RX_OK, AccessibilityCheckMode::AT_EACCESS);
	}
	static std::error_code IsPathReadableAndWritable4EID(char const *pathname)
	{
		return IsPathAccessible(
				AtFlags::AT_FDCWD, pathname, Accessibility::RW_OK, AccessibilityCheckMode::AT_EACCESS);
	}
	static std::error_code IsPathFullyAccessible4EID(char const *pathname)
	{
		return IsPathAccessible(
				AtFlags::AT_FDCWD, pathname, Accessibility::RWX_OK, AccessibilityCheckMode::AT_EACCESS);
	}


	/**
	 * Linux system call semantics:
	 *     Reposition the file offset to "origin:offset".
	 *     It's OK to go beyond EOF. After this done, the file size won't be changed until a real
	 *     write done. When write done there, a hole created and zero will be read from this hole
	 *     region until data written into this region that will fill out the hole.
	 *     ESPIPE if fd references a pipe, socket or FIFO.
	 *     If O_APPEND is used when open a file, this operation has no effect.
	 *
	 * Linux system call synopsis:  // <KernelRoot>/fs/read_write.c
	 *     long sys_lseek(unsigned fd, off_t offset, unsgined origin);
	 *
	 * Linux system headers:  // __NR_lseek 8
	 *     unistd.h
	 * Glibc Synopsis:  // !defined(__USE_FILE_OFFSET64)
	 *     off_t lseek(int fd, off_t offset, int origin);
	 * Glibc Synopsis:  // defined(__USE_FILE_OFFSET64) && defined(__REDIRECT_NTH)
	 *     off64_t lseek(int fd, off64_t offset, int origin);
	 * Glibc Synopsis:  // defined(__USE_LARGEFILE64)
	 *     off64_t lseek64(int fd, off64_t offset, int origin);
	 */
	static std::error_code Seek(int fd, SeekOrigin origin, off_t offset);

	/**
	 * Linux system call semantics:
	 *	   Close the file handle for reusability. Release traditional record locks associated with
	 *	   the target file and owned by this process even if locks are not gotten with this file
	 *	   handle.
	 *	   If this file handle is the last one on the corresponding file description,
	 *	   resources to the file description will be released.
	 *	   If this file handle is the last refernece to the file after unlinked, the file will be
	 *	   deleted also.
	 *	   For Linux, concurrent thread might write the closed file due to file description
	 *	   referenced by the IO.
	 *	   For Linux, close will guarantee successful release of file handle even under some error.
	 *	   So, don't try to reclose a file handle under error condition. This behavior is on the way
	 *	   to POSIX standard behavior.
	 *     Possible error code: EBADF|EINTR|EIO|ENOSPC|EDQUOT
	 *     To make sure that the close operation catchs all possilbe error, invoke fsync before
	 *     close.
	 *     Check error code of close for rewriting file data, backup or just reporting error to
	 *     avoid quiet lost of data.
	 *
	 * Linux system headers:  // __NR_close 3
	 *     unistd.h
	 * Glibc Synopsis:
	 *     int close(int fd);
	 * Glibc synopsis:  // __USE_MISC
	 *     void closefrom(int fd);
	 */
	static std::error_code Close(int fd);
	static std::error_code CloseFrom(int fdStart);
};  // class FileSystemAPI

class ProcessManager {
public:
	/**
	 * Headers:  // __NR_execve 59
	 *     unistd.h
	 * Synopsis:
	 *     int execve(char const *pathname, char *const argv[], char *const envp[]);
	 * Headers:  // __NR_execveat 322
	 *     linux/fcntl.h  // AT_* for flags
	 *     unistd.h
	 * Synopsis:
	 *     int execveat(int dirfd, char const *pathname, char const *const argv[], char const *const envp[], int flags);
	 */

	/**
	 * Linux system call semantics:
	 *
	 * Linux system call synopsis:
	 *     // __NR_pidfd_open 434; <KernelRoot>/kernel/pid.c; kernel-5.3
	 *
	 * Linux system headers:
	 *
	 * Glibc synopsis:
	 *
	 * Notes on Linux implementation:
	 *
	 */

};

class InterProcessCommunication {
public:
	/**
	 * Linux system call semantics:
	 *
	 * Linux system call synopsis:
	 *     // __NR_mq_open 240; <KernelRoot>/ipc/mqueue.c
	 *     long sys_mq_open(char const *uname, int oflags, umode_t mode, struct mq_attr *uattr);
	 *
	 * Linux system headers:
	 *
	 * Glibc synopsis:
	 *
	 * Notes on Linux implementation:
	 *
	 */

};

class TraceAndMotitor {
public:
	/**
	 * Linux system call semantics:
	 *
	 * Linux system call synopsis:
	 *     // __NR_perf_event_open 298; <KernelRoot>/kernel/events/core.c
	 *     long sys_perf_event_open(struct perf_event_attr *attr_uptr, pid_t pid, int cpu, int groupid, unsigned flags);
	 *
	 * Linux system headers:
	 *
	 * Glibc synopsis:
	 *
	 * Notes on Linux implementation:
	 *
	 */

};

}  // namespace posix
}  // namespace systemcall
}  // namespace liuzan



#endif  // LIUZAN_SYSTEMCALL_POSIX_API_HPP
