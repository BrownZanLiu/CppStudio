#ifndef LIUZAN_FSTEST_FLAGS_H
#define LIUZAN_FSTEST_FLAGS_H

#include <gflags/gflags.h>

/** ==== General flags ==== **/
DECLARE_bool(debug);
DECLARE_string(root);
DECLARE_string(pathname);

/** ==== Flags for C++ std filesystem api tests ==== **/

/** ==== Flags for Glibc file system api tests ==== **/

#endif // LIUZAN_FSTEST_FLAGS_H
