#include <cstring>
#include <iostream>
#include <gtest/gtest.h>

int FindCurrentMaxLength(bool const **matrix, int const N, int const R, int const C)
{
	bool **vTrace = new bool[N][N];
	std::memset(vTrace, 0, sizeof(vTrace));

	std::cout << "sizeof(vTrace) = " << sizeof(vTrace) << std::endl;

	return 3;
}

int FindGlobalMaxLength(bool const **matrix, int const N)
{
	int vMaxLength = 0;

	for (int c = 0; c < N; ++c) {
		for (int r = 0; r < N; ++r) {
			if (!matrix[c][r]) continue;
			int vLen = FindCurrentMaxLength(matrix, N, r, c);
			if (vLen > vMaxLength) vMaxLength = vLen;
		}
	}

	return vMaxLength;
}

TEST(TSRecursionBacktrace, TCFindGlobalMaxLength)
{
	bool vMatrix[3][3] = {
		{true, false, false},
		{false, true, false},
		{false, false, true}
	};

	EXPECT_TRUE(FindGlobalMaxLength(vMatrix, 3) == 3);
}
