#include <gflags/gflags.h>

/** ==== General flags ==== **/
DEFINE_bool(debug, false, "Build a debug version.");
DEFINE_string(root, "/mnt/fstest", "Specify the root directory where fstest will run.");
DEFINE_string(pathname, "", "Specify the target file/directory pathname.");


/** ==== Flags for C++ std filesystem api tests ==== **/

/** ==== Flags for Glibc file system api tests ==== **/

