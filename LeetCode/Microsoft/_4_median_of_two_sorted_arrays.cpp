/*
Given two sorted arrays nums1 and nums2 of size m and n respectively, return the median of the two sorted arrays.

The overall run time complexity should be O(log (m+n)).

Example 1:

Input: nums1 = [1,3], nums2 = [2]
Output: 2.00000
Explanation: merged array = [1,2,3] and median is 2.
Example 2:

Input: nums1 = [1,2], nums2 = [3,4]
Output: 2.50000
Explanation: merged array = [1,2,3,4] and median is (2 + 3) / 2 = 2.5.
 

Constraints:

nums1.length == m
nums2.length == n
0 <= m <= 1000
0 <= n <= 1000
1 <= m + n <= 2000
-106 <= nums1[i], nums2[i] <= 106

来源：力扣（LeetCode）
链接：https://leetcode.cn/problems/median-of-two-sorted-arrays
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。
*/

/**
 * Copyright: brown.liuzan@outlook.com
 */

#include <vector>
#include <limits>

#include <dynamic_assert.h>
#include <gtest/gtest.h>

namespace liuzan {
namespace leetcode {
namespace microsoft {

class Cursor {
public:
	Cursor(std::vector<int> const *vec, int vecIndex): mVec(vec), mVecIndex(vecIndex) {}

	/**
	 * No boundary check hear for simplicity.
	 */
	bool IsNull() const { return mVec == nullptr; }
	void SetNull() { mVec = nullptr; mVecIndex = -1; }
	int GetValue() const { return mVec->at(mVecIndex); }
	int GetValueByIndex(int index) const  { return mVec->at(index); }
	int GetValueByOffset(int offset) const { return mVec->at(mVecIndex+offset); }
	int GetIndex() const  { return mVecIndex; }
	int GetSize() const { return mVec->size(); }
	void SetIndex(int index)
	{
		liuzan::DynamicAssert(index < mVec->size(), "Index[{}] >= Size[{}]", index, mVec->size());
		mVecIndex = index;
	}
	void GoForwardOneStep() { ++mVecIndex; }
	void GoForwardTo(int targetVal)
	{
		int vMaxIndex = mVec->size();
		int vIndex = mVecIndex + (vMaxIndex - mVecIndex + 1) / 2;
		while (true) {
			if (vIndex >= vMaxIndex) {
				break;
			}
			if (GetValueByIndex(vIndex) < targetVal) {
				SetIndex(vIndex);
			} else {
				vMaxIndex = vIndex;
			}
			vIndex = mVecIndex + (vMaxIndex - mVecIndex + 1) / 2;
		}
	}
	void GoBackwardOneStep() { --mVecIndex; }
	void GoBackwardTo(int targetVal, int maxOff = std::numeric_limits<int>::max())
	{
		int vMinIndex = -1;
		if (mVecIndex - vMinIndex > maxOff) {
			vMinIndex = mVecIndex - maxOff;
		}
		int vIndex = mVecIndex - (mVecIndex - vMinIndex + 1) / 2;
		while (true) {
			if (vIndex <= vMinIndex) {
				break;
			}
			if (GetValueByIndex(vIndex) > targetVal) {
				SetIndex(vIndex);
			} else {
				vMinIndex = vIndex;
			}
			vIndex = mVecIndex - (mVecIndex - vMinIndex + 1) / 2;
		}
	}

private:
	std::vector<int> const *mVec;
	int mVecIndex;
};

double MedianOf2SortedArrays(std::vector<int> const & nums1, std::vector<int> const & nums2)
{
	auto Median = [](int a, int b) { return (static_cast<double>(a) + static_cast<double>(b) ) / 2.0; };
	auto Swap = [](Cursor **first, Cursor **second) {
		auto pinter = *first;
		*first = *second;
		*second = pinter;
	};
	auto GlobalIndex4High = [](Cursor const *low, Cursor const *high) {
		if (low->IsNull()) {
			return high->GetIndex();
		} else if (high->IsNull()) {
			return low->GetIndex();
		} else {
			return low->GetIndex() + 1 + high->GetIndex();
		}
	};
	auto GoBackwardGlobally = [Swap](Cursor **cursor1, Cursor **cursor2) {
		Cursor *vpCursor1 = *cursor1;
		Cursor *vpCursor2 = *cursor2;
		bool vBackwardDone2 = false;
		if (vpCursor2->GetIndex() > 0) {
			vpCursor2->GoBackwardOneStep();
			vBackwardDone2 = true;
		} else {
			vpCursor2->SetNull();
			Swap(cursor1, cursor2);
			return;
		}
		if (vpCursor1->GetValue() > vpCursor2->GetValue()) {
			Swap(cursor1, cursor2);
		}
	};
	auto GoForwardGlobally = [Swap](Cursor **cursor1, Cursor **cursor2) {
		Cursor *vpCursor1 = *cursor1;
		Cursor *vpCursor2 = *cursor2;
		bool vForwardDone1 = false;
		bool vForwardDone2 = false;
		if ((vpCursor1->GetIndex() + 1) < vpCursor1->GetSize()) {
			vpCursor1->GoForwardOneStep();
			vForwardDone1 = true;
		}
		if ((vpCursor2->GetIndex() + 1) < vpCursor2->GetSize()) {
			vpCursor2->GoForwardOneStep();
			vForwardDone2 = true;
		}
		if (vpCursor1->GetValue() < vpCursor2->GetValue()) {
			if (vForwardDone1 && vForwardDone2) {
				vpCursor2->GoBackwardOneStep();
			}
			Swap(cursor1, cursor2);
		} else {
			if (vForwardDone1 && vForwardDone2) {
				vpCursor1->GoBackwardOneStep();
			}
		}
	};

	/**
	 * nums1.size: N = 2n + 1 || 2n; 0 <= N <= 1000
	 * nums2.size: M = 2m + 1 || 2m; 0 <= M <= 1000
	 * case 1: nums1.size == 0
	 *     GLOBAL_TARGET_INDEX1 == (M - 1) / 2
	 *     GLOBAL_TARGET_INDEX2 == M / 2
	 * case 2: nums2.size == 0
	 *     GLOBAL_TARGET_INDEX1 == (N - 1) / 2
	 *     GLOBAL_TARGET_INDEX2 == N / 2
	 * case 3: nums1.size == 2n && nums2.size == 2m
	 *     GLOBAL_TARGET_INDEX1 == n + m -1
	 *     GLOBAL_TARGET_INDEX2 == n + m
	 * case 4: nums1.size == 2n + 1 && nums2.size == 2m
	 *     GLOBAL_TARGET_INDEX1 == n + m
	 *     GLOBAL_TARGET_INDEX2 == n + m
	 * case 5: nums1.size == 2n & nums2.size == 2m + 1
	 *     GLOBAL_TARGET_INDEX1 == n + m
	 *     GLOBAL_TARGET_INDEX2 == n + m
	 * case 6: nums1.size == 2n + 1 && nums2.size == 2m + 1
	 *     GLOBAL_TARGET_INDEX1 == n + m
	 *     GLOBAL_TARGET_INDEX2 == n + m + 1
	 */
	const int GLOBAL_TARGET_INDEX1 = (nums1.size() + nums2.size() - 1) / 2;
	const int GLOBAL_TARGET_INDEX2 = (nums1.size() + nums2.size()) / 2;

	if (nums1.size() == 0) {
		std::cout << "First vector is empty." << std::endl;
		return Median(nums2[GLOBAL_TARGET_INDEX1], nums2[GLOBAL_TARGET_INDEX2]);
	} else if (nums2.size() == 0) {
		std::cout << "Second vector is empty." << std::endl;
		return Median(nums1[GLOBAL_TARGET_INDEX1], nums1[GLOBAL_TARGET_INDEX2]);
	}

	auto vpCursor1 = new Cursor(&nums1, (nums1.size() - 1) / 2);
	auto vpCursor2 = new Cursor(&nums2, (nums2.size() - 1) / 2);
	if (vpCursor1->GetValue() > vpCursor2->GetValue()) {
		Swap(&vpCursor1, &vpCursor2);
	}

	std::cout << "Before adustment: index1 = "
		<< vpCursor1->GetIndex() << ", index2 = " << vpCursor2->GetIndex() << std::endl;
	vpCursor1->GoForwardTo(vpCursor2->GetValue());
	std::cout << "After adustment1: index1 = "
		<< vpCursor1->GetIndex() << ", index2 = " << vpCursor2->GetIndex() << std::endl;
	if (GlobalIndex4High(vpCursor1, vpCursor2) > GLOBAL_TARGET_INDEX2) {
		vpCursor2->GoBackwardTo(
				vpCursor1->GetValue(),
				GlobalIndex4High(vpCursor1, vpCursor2) - GLOBAL_TARGET_INDEX2
				);
	}
	std::cout << "After adustment2: index1 = "
		<< vpCursor1->GetIndex() << ", index2 = " << vpCursor2->GetIndex() << std::endl;

	while(GlobalIndex4High(vpCursor1, vpCursor2) < GLOBAL_TARGET_INDEX1) {
		GoForwardGlobally(&vpCursor1, &vpCursor2);
	}
	std::cout << "After adustment3: index1 = "
		<< vpCursor1->GetIndex() << ", index2 = " << vpCursor2->GetIndex() << std::endl;

	while(!vpCursor1->IsNull() && GlobalIndex4High(vpCursor1, vpCursor2) > GLOBAL_TARGET_INDEX2) {
		GoBackwardGlobally(&vpCursor1, &vpCursor2);
	}
	std::cout << "After adustment4: index1 = "
		<< vpCursor1->GetIndex() << ", index2 = " << vpCursor2->GetIndex() << std::endl;

	liuzan::DynamicAssert(!vpCursor2->IsNull(), "vpCursor2 shouldn't be nullptr.");
	if (vpCursor1->IsNull()) {
		std::cout << "vpCursor1 is null: mVecIndex = " << vpCursor1->GetIndex() << std::endl;
	}
	int vVal1, vVal2;
	if (vpCursor1->IsNull()) {
		vVal1 = vpCursor2->GetValueByIndex(GLOBAL_TARGET_INDEX1);
		vVal2 = vpCursor2->GetValueByIndex(GLOBAL_TARGET_INDEX2);
	} else if (GLOBAL_TARGET_INDEX1 == GLOBAL_TARGET_INDEX2) {
		std::cout << "Identical indexes[" << GLOBAL_TARGET_INDEX1 << "], val = " << vpCursor2->GetValue() << std::endl;
		return vpCursor2->GetValue();
	} else if (GlobalIndex4High(vpCursor1, vpCursor2) == GLOBAL_TARGET_INDEX1) {
		vVal1 = vpCursor2->GetValue();
		GoForwardGlobally(&vpCursor1, &vpCursor2);
		vVal2 = vpCursor2->GetValue();
	} else {
		vVal2 = vpCursor2->GetValue();
		GoBackwardGlobally(&vpCursor1, &vpCursor2);
		vVal1 = vpCursor2->GetValue();
	}

	std::cout << "(" << vVal1 << ", " << vVal2 << ")" << std::endl;
	return Median(vVal1, vVal2);
}

TEST(TSLeetCode, TCMedianOf2SortedArrays)
{
	std::vector<int> vNums1{1, 3, 5, 7, 11, 13, 17};
	std::vector<int> vNums2{};
	EXPECT_FLOAT_EQ(MedianOf2SortedArrays(vNums1, vNums2), 7.0);

	vNums1 = {};
	vNums2 = {2};
	EXPECT_FLOAT_EQ(MedianOf2SortedArrays(vNums1, vNums2), 2.0);

	vNums1 = {};
	vNums2 = {2, 4};
	EXPECT_FLOAT_EQ(MedianOf2SortedArrays(vNums1, vNums2), 3.0);

	vNums1 = {};
	vNums2 = {2, 4, 9};
	EXPECT_FLOAT_EQ(MedianOf2SortedArrays(vNums1, vNums2), 4.0);

	vNums1 = {};
	vNums2 = {2, 4, 9, 90};
	EXPECT_FLOAT_EQ(MedianOf2SortedArrays(vNums1, vNums2), 6.5);

	vNums1 = {1, 3};
	vNums2 = {2};
	EXPECT_FLOAT_EQ(MedianOf2SortedArrays(vNums1, vNums2), 2.0);

	vNums1 = {1, 2};
	vNums2 = {3, 4};
	EXPECT_FLOAT_EQ(MedianOf2SortedArrays(vNums1, vNums2), 2.5);

	vNums1 = {1, 3, 5, 7, 11, 13, 17};
	vNums2 = {2, 4, 7};
	EXPECT_FLOAT_EQ(MedianOf2SortedArrays(vNums1, vNums2), 6.0);

	vNums1 = {1, 3, 5, 7, 11, 11, 11, 11, 13, 17};
	vNums2 = {2, 4, 7, 18, 18, 18, 18, 18, 18, 19, 20, 21, 22};
	EXPECT_FLOAT_EQ(MedianOf2SortedArrays(vNums1, vNums2), 13.0);

	vNums1 = {3};
	vNums2 = {-2, -1};
	EXPECT_FLOAT_EQ(MedianOf2SortedArrays(vNums1, vNums2), -1.0);
}

/**
 * Commit result on LeetCode: https://leetcode.cn/problems/median-of-two-sorted-arrays/submissions/
 * 执行用时：12 ms, 在所有 C++ 提交中击败了99.48%的用户
 * 内存消耗：87.3 MB, 在所有 C++ 提交中击败了38.64%的用户
 * 通过测试用例：2094 / 2094
 */
class Solution {
public:
	double findMedianSortedArrays(std::vector<int> const &nums1, std::vector<int> const &nums2) {
		auto Median = [](int a, int b) { return (static_cast<double>(a) + static_cast<double>(b) ) / 2.0; };
		auto Swap = [](Cursor **first, Cursor **second) {
			auto pinter = *first;
			*first = *second;
			*second = pinter;
		};
		auto GlobalIndex4High = [](Cursor const *low, Cursor const *high) {
			if (low->IsNull()) {
				return high->GetIndex();
			} else if (high->IsNull()) {
				return low->GetIndex();
			} else {
				return low->GetIndex() + 1 + high->GetIndex();
			}
		};
		auto GoBackwardGlobally = [Swap](Cursor **cursor1, Cursor **cursor2) {
			Cursor *vpCursor1 = *cursor1;
			Cursor *vpCursor2 = *cursor2;
			bool vBackwardDone2 = false;
			if (vpCursor2->GetIndex() > 0) {
				vpCursor2->GoBackwardOneStep();
				vBackwardDone2 = true;
			} else {
				vpCursor2->SetNull();
				Swap(cursor1, cursor2);
				return;
			}
			if (vpCursor1->GetValue() > vpCursor2->GetValue()) {
				Swap(cursor1, cursor2);
			}
		};
		auto GoForwardGlobally = [Swap](Cursor **cursor1, Cursor **cursor2) {
			Cursor *vpCursor1 = *cursor1;
			Cursor *vpCursor2 = *cursor2;
			bool vForwardDone1 = false;
			bool vForwardDone2 = false;
			if ((vpCursor1->GetIndex() + 1) < vpCursor1->GetSize()) {
				vpCursor1->GoForwardOneStep();
				vForwardDone1 = true;
			}
			if ((vpCursor2->GetIndex() + 1) < vpCursor2->GetSize()) {
				vpCursor2->GoForwardOneStep();
				vForwardDone2 = true;
			}
			if (vpCursor1->GetValue() < vpCursor2->GetValue()) {
				if (vForwardDone1 && vForwardDone2) {
					vpCursor2->GoBackwardOneStep();
				}
				Swap(cursor1, cursor2);
			} else {
				if (vForwardDone1 && vForwardDone2) {
					vpCursor1->GoBackwardOneStep();
				}
			}
		};
		const int GLOBAL_TARGET_INDEX1 = (nums1.size() + nums2.size() - 1) / 2;
		const int GLOBAL_TARGET_INDEX2 = (nums1.size() + nums2.size()) / 2;

		if (nums1.size() == 0) {
			return Median(nums2[GLOBAL_TARGET_INDEX1], nums2[GLOBAL_TARGET_INDEX2]);
		} else if (nums2.size() == 0) {
			return Median(nums1[GLOBAL_TARGET_INDEX1], nums1[GLOBAL_TARGET_INDEX2]);
		}

		auto vpCursor1 = new Cursor(&nums1, (nums1.size() - 1) / 2);
		auto vpCursor2 = new Cursor(&nums2, (nums2.size() - 1) / 2);
		if (vpCursor1->GetValue() > vpCursor2->GetValue()) {
			Swap(&vpCursor1, &vpCursor2);
		}

		vpCursor1->GoForwardTo(vpCursor2->GetValue());
		if (GlobalIndex4High(vpCursor1, vpCursor2) > GLOBAL_TARGET_INDEX2) {
			vpCursor2->GoBackwardTo(
					vpCursor1->GetValue(),
					GlobalIndex4High(vpCursor1, vpCursor2) - GLOBAL_TARGET_INDEX2
					);
		}

		while(GlobalIndex4High(vpCursor1, vpCursor2) < GLOBAL_TARGET_INDEX1) {
			GoForwardGlobally(&vpCursor1, &vpCursor2);
		}

		while(!vpCursor1->IsNull() && GlobalIndex4High(vpCursor1, vpCursor2) > GLOBAL_TARGET_INDEX2) {
			GoBackwardGlobally(&vpCursor1, &vpCursor2);
		}

		int vVal1, vVal2;
		if (vpCursor1->IsNull()) {
			vVal1 = vpCursor2->GetValueByIndex(GLOBAL_TARGET_INDEX1);
			vVal2 = vpCursor2->GetValueByIndex(GLOBAL_TARGET_INDEX2);
		} else if (GLOBAL_TARGET_INDEX1 == GLOBAL_TARGET_INDEX2) {
			return vpCursor2->GetValue();
		} else if (GlobalIndex4High(vpCursor1, vpCursor2) == GLOBAL_TARGET_INDEX1) {
			vVal1 = vpCursor2->GetValue();
			GoForwardGlobally(&vpCursor1, &vpCursor2);
			vVal2 = vpCursor2->GetValue();
		} else {
			vVal2 = vpCursor2->GetValue();
			GoBackwardGlobally(&vpCursor1, &vpCursor2);
			vVal1 = vpCursor2->GetValue();
		}

		return Median(vVal1, vVal2);
	}
};

}  // namespace microsoft
}  // namespace leetcode
}  // namespace liuzan
