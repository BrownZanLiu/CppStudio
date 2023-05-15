#include<iostream>

#include "demo.h"

namespace liuzan {
namespace demo {
	template<typename CharT, typename Char_Traits = std::char_traits<CharT>>
	std::basic_ostream<CharT, Char_Traits>& operator <<(
			std::basic_ostream<CharT, Char_Traits> &os,
			const CmakeDemo & obj
			)
	{
		os << obj.Major()
		   << '.'
		   << obj.Minor()
		   << '.'
		   << obj.Patch()
		   << '.'
		   << obj.Tweak();

		return os;
	}
}
}

int main(int argc, char **argv)
{
	liuzan::demo::CmakeDemo vCMakeDemo{3, 26, 3};

	std::cout << "cmake version: " << vCMakeDemo << std::endl;
	std::cout << "C++ version: " << __cplusplus << std::endl;

	return 0;
}
