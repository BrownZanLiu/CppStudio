#include <iostream>
#include <vector>

#include <gtest/gtest.h>

TEST(TestVector, Demo)
{
	std::vector<int> vInts{1, 2, 3, 4};

	std::cout << "Array size: " << vInts.size() << std::endl;
	std::cout << "Array content: " << std::endl;
	for (auto const & i : vInts) {
		std::cout << i << " ";
	}
	std::cout << std::endl;
}
