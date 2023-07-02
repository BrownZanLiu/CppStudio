/**
	Given an array of integers \a nums and an integer target \a sum, return indices of the two
	numbers such that they add up to target.

	You may assume that each input would have exactly one solution, and you may not use the same
	element twice.

	You can return the answer in any order.

	Example 1:
	Input: nums = [2,7,11,15], target = 9
	Output: [0,1]
	Explanation: Because nums[0] + nums[1] == 9, we return [0, 1].

	Example 2:
	Input: nums = [3,2,4], target = 6
	Output: [1,2]

	Example 3:
	Input: nums = [3,3], target = 6
	Output: [0,1]
	 
	Constraints:
	2 <= nums.length <= 104
	-109 <= nums[i] <= 109
	-109 <= target <= 109
	Only one valid answer exists.
	 

	Follow-up: Can you come up with an algorithm that is less than O(n^2) time complexity?

	来源：力扣（LeetCode）
	链接：https://leetcode.cn/problems/two-sum
	著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。
*/

/**
 * Copyright: brown.liuzan@outlook.com
 */

#include <map>
#include <vector>
#include <utility>

#include <dynamic_assert.h>
#include <gtest/gtest.h>

namespace liuzan {
namespace leetcode {
namespace microsoft {

std::pair<int,int> Find2Elems4Sum(std::vector<int> nums, int sum)
{
	std::map<int, int> vInteger2Index;

	int i = 0;
	vInteger2Index.emplace(nums[i], i);
	int tgt;
	int index;
	for (++i; i < nums.size(); ++i) {
		tgt = sum - nums[i];
		auto it = vInteger2Index.find(tgt);
		if (it == vInteger2Index.end()) {
			vInteger2Index.emplace(nums[i], i);
		} else {
			return std::make_pair(it->second, i);
		}
	}

	return std::make_pair<int, int>(-1, -1);
}

TEST(TSMicrosoft, TCFind2Elems4sum)
{
	std::vector<int> nums1{2,7,11,15};
	int sum1{9};
	auto result1= Find2Elems4Sum(nums1, sum1);
	EXPECT_EQ(result1, std::make_pair(0, 1));

	std::vector<int> nums2{3, 2, 4};
	int sum2{6};
	auto result2= Find2Elems4Sum(nums2, sum2);
	EXPECT_EQ(result2, std::make_pair(1, 2));

	std::vector<int> nums3{3, 3};
	int sum3{6};
	auto result3= Find2Elems4Sum(nums3, sum3);
	EXPECT_EQ(result3, std::make_pair(0, 1));
}

}  // namesapce microsoft
}  // namespace leetcode
}  // namespace liuzan
