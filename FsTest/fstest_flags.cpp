#include <gflags/gflags.h>

/** ==== General flags ==== **/
/**
 * By default, all flags are false or zero values and flag users define the actual default values in their scenarioes.
 * Use command-line arguments to modify default behaviors.
 */
DEFINE_bool(debug, false, "Build a debug version.");
DEFINE_string(logroot, "", "Specify the root directory of logging where fstest will run.");
DEFINE_string(root, "", "Specify the root directory where fstest will run.");
DEFINE_string(pathname, "", "Specify filesystem target on which we will do work.");
DEFINE_string(name_prefix, "", "Specify basename prefix of targets.");
DEFINE_uint32(jobnum, 0u, "Specify number of threads to do works.");
DEFINE_uint32(treedepth, 0u, "Specify number of layers of the target directory tree.");
DEFINE_uint32(treeout, 0u, "Specify number of directory children.");
DEFINE_uint32(treeout_min, 0u, "Specify minimum directory children.");
DEFINE_uint32(treeout_max, 0u, "Specify maximum directory children.");
DEFINE_uint32(dirnum, 0llu, "Specify directories to create.");
DEFINE_uint64(filesperdir, 0llu, "Specify number of files per directory.");
DEFINE_uint64(filesperdir_min, 0llu, "Specify minimum files per directory.");
DEFINE_uint64(filesperdir_max, 0llu, "Specify maximum files per directory.");
DEFINE_uint64(filenum, 0llu, "Specify files to create.");
DEFINE_uint64(filesize, 0llu, "Specify file size in bytes");
DEFINE_uint64(filesize_min, 0llu, "Specify minimum file size in bytes.");
DEFINE_uint64(filesize_max, 0llu, "Specify maximum file size in bytes.");


/** ==== Flags for C++ std filesystem api tests ==== **/

/** ==== Flags for Glibc file system api tests ==== **/

