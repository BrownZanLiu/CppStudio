#ifndef LIUZAN_FSTEST_FLAGS_H
#define LIUZAN_FSTEST_FLAGS_H

#include <gflags/gflags.h>

/** ==== General flags ==== **/
DECLARE_bool(debug);
DECLARE_string(root);
DECLARE_string(logroot);
DECLARE_string(pathname);
DECLARE_uint32(jobnum);
DECLARE_uint32(treedepth);
DECLARE_uint32(treeout);
DECLARE_uint32(treeout_min);
DECLARE_uint32(treeout_max);
DECLARE_uint32(dirnum);
DECLARE_uint64(filesperdir);
DECLARE_uint32(filesperdir_min);
DECLARE_uint32(filesperdir_max);
DECLARE_uint64(filenum);
DECLARE_uint64(filesize);
DECLARE_uint64(filesize_min);
DECLARE_uint64(filesize_max);

/** ==== Flags for C++ std filesystem api tests ==== **/

/** ==== Flags for Glibc file system api tests ==== **/

#endif // LIUZAN_FSTEST_FLAGS_H
