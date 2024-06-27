#ifndef LIUZAN_PERCPU_VAR_H
#define LIUZAN_PERCPU_VAR_H

#include <cstdint>  // uint64_t
#include <cstring>  // memset()

#include <unistd.h>  //  sysconf(_SC_NPROCESSORS_ONLN)

namespace liuzan {
template<typename T>
class PerCpuVar {
/**
 * Notes:
 * - T must support "operator +(const T&)"
 * - This template has no guarantee to avoid cross-cpu reference if thread sleeps and wakes up on another cpu after
 *   invocation of CurCpuPart().
 * - No like the kernel counterpart, this implementation is not very memory efficient if T is some object type.
 */
public:
	PerCpuVar()
		: NR_CPUS(sysconf(_SC_NPROCESSORS_ONLN))
	{
		mVal = new percpu_value[NR_CPUS];
		memset(mVal, 0, sizeof(percpu_value) * NR_CPUS);
	}
	~PerCpuVar()
	{
		delete[] mVal;
	}
	PerCpuVar(const PerCpuVar &right) = delete;
	PerCpuVar& operator =(const PerCpuVar &right) = delete;

protected:
	T &curCpuPart()
	{
		return mVal[sched_getcpu()].cpuVal;
	}

	T &cpuPart(int i)
	{
		return mVal[i].cpuVal;
	}

	int nrOfCpus() const
	{
		return NR_CPUS;
	}

	void clearAll()
	{
		memset(mVal, 0, sizeof(percpu_value) * NR_CPUS);
	}

private:
	static constexpr int CACHELINE_BYTES = 64;
	struct percpu_value {
		alignas(CACHELINE_BYTES) T cpuVal;
	};

	const int NR_CPUS;
	percpu_value *mVal;
};

}  // namespace liuzan
#endif  // LIUZAN_PERCPU_VAR_H
