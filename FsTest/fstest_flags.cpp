#include <gflags/gflags.h>

/** ==== General flags ==== **/
DEFINE_bool(debug, false, "Build a debug version.");
DEFINE_bool(shareddir, false, "If all threads do works under the same directory or not?");
DEFINE_string(root, "/mnt/fstest", "Specify the root directory where fstest will run.");
DEFINE_string(pathname, "", "Specify the target file/directory pathname. Both relative and absolute path are ok. NO ~");
DEFINE_uint32(jobnum, 1u, "Specify number of threads to do works.");
DEFINE_uint32(treedepth, 1u, "Specify number of layers of the target directory tree.");
DEFINE_uint32(treeout, 1u, "Specify number of directory children.");
DEFINE_uint32(treeout_min, 1u, "Specify minimum directory children.");
DEFINE_uint32(treeout_max, 1u, "Specify maximum directory children.");
DEFINE_uint32(dirnum, 50000llu, "Specify directories to create.");
DEFINE_uint64(filesperdir, 50000llu, "Specify number of files per directory.");
DEFINE_uint64(filesperdir_min, 1llu, "Specify minimum files per directory.");
DEFINE_uint64(filesperdir_max, 50000llu, "Specify maximum files per directory.");
DEFINE_uint64(filenum, 50000llu, "Specify files to create.");
DEFINE_uint64(filesize, 4096llu, "Specify file size in bytes");
DEFINE_uint64(filesize_min, 256llu, "Specify minimum file size in bytes.");
DEFINE_uint64(filesize_max, 17179869184llu, "Specify maximum file size in bytes.");


/** ==== Flags for C++ std filesystem api tests ==== **/

/** ==== Flags for Glibc file system api tests ==== **/

