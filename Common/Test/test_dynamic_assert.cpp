#include <iostream>

#include <dynamic_assert.hpp>
#include <gtest/gtest.h>

TEST(TSCommon, TCDynamicAssert)
{
	using namespace liuzan;

	int a = 1, b = 1;

	try {
		DynamicAssert("true" == "true");
		DynamicAssert(1 == 1, "1!=1");
		DynamicAssert(a == b, "{} != {}", a, b);
	} catch (DynamicAssertionFailure &e) {
		std::cout << e.what() << std::endl;
		EXPECT_TRUE(false);
	}

	try {
		DynamicAssert("true" == "false");
		EXPECT_TRUE(false);
	} catch (DynamicAssertionFailure &e) {
		std::cout << e.what() << std::endl;
		EXPECT_TRUE(true);
	}

	try {
		DynamicAssert(1 == 2, "1 != 2");
		EXPECT_TRUE(false);
	} catch (DynamicAssertionFailure &e) {
		std::cout << e.what() << std::endl;
		EXPECT_TRUE(true);
	}

	try {
		b = 2;
		DynamicAssert(a == b, "{} != {}", a, b);
		EXPECT_TRUE(false);
	} catch (DynamicAssertionFailure &e) {
		std::cout << e.what() << std::endl;
		EXPECT_TRUE(true);
	}
}


