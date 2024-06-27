#ifndef LIUZAN_HOST_INFO_H
#define LIUZAN_HOST_INFO_H

#include <unistd.h>  // sysconf(_SC_NPROCESSORS_ONLN)
namespace liuzan {

class HostInfo {
public:
	static int OnlineCpus()
	{
		return sysconf(_SC_NPROCESSORS_ONLN);
	}
};

}  // namespace liuzan

#endif  // LIUZAN_HOST_INFO_H
