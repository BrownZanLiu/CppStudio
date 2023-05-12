#ifndef CPPSTUDIO_DEMO_H
#define CPPSTUDIO_DEMO_H

#include <ostream>
#include <string>

namespace liuzan {
namespace demo {
	class CmakeDemo {
	private:
		unsigned mMajorVersion;
		unsigned mMinorVersion;
		unsigned mPatch;
		unsigned mTweak;
	public:
		CmakeDemo(unsigned major, unsigned minor, unsigned patch = 0, unsigned tweak = 0):
			mMajorVersion(major),
			mMinorVersion(minor),
			mPatch(patch),
			mTweak(tweak)
		{}

		CmakeDemo(const CmakeDemo &orgin) = default;

		CmakeDemo & operator =(const CmakeDemo &orgin) = default;

		~CmakeDemo() = default;

		unsigned Major() const { return mMajorVersion; }
		unsigned Minor() const { return mMinorVersion; }
		unsigned Patch() const { return mPatch; }
		unsigned Tweak() const { return mTweak; }
	};
}
}

#endif  // CPPSTUDIO_DEMO_H
