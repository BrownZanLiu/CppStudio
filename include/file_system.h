#ifndef LIUZAN_FILE_SYSTEM_H

#include <cstdint>  // uint32_t, uint64_t
#include <string>  // std::string

#include <errno.h>
#include <fcntl.h>  // open(), AT_*
#include <string.h>  // strerror()
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

}  // namespace filesystem
}  // namespace liuzan

#endif  // LIUZAN_FILE_SYSTEM_H
