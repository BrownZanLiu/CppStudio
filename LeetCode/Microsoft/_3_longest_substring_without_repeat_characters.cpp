/**
Given a string s, find the length of the longest substring without repeating characters.

Example 1:

Input: s = "abcabcbb"
Output: 3
Explanation: The answer is "abc", with the length of 3.
Example 2:

Input: s = "bbbbb"
Output: 1
Explanation: The answer is "b", with the length of 1.
Example 3:

Input: s = "pwwkew"
Output: 3
Explanation: The answer is "wke", with the length of 3.
Notice that the answer must be a substring, "pwke" is a subsequence and not a substring.
 

Constraints:

0 <= s.length <= 5 * 104
s consists of English letters, digits, symbols and spaces.

来源：力扣（LeetCode）
链接：https://leetcode.cn/problems/longest-substring-without-repeating-characters
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。
*/

/**
 * Copyright: brown.liuzan@outlook.com
 */

#include <array>
#include <string>

#include <gtest/gtest.h>

namespace liuzan {
namespace leetcode {
namespace microsoft {

int LengthOfLongestSubstring(std::string const &str)
{
	constexpr int CHAR_SET_SIZE = 256;
	std::array<char, CHAR_SET_SIZE> vHitMap;
	int vMaxLen = 0;

	auto vItStart = str.cbegin();
	while (vItStart != str.cend()) {
		vHitMap.fill(0);
		vHitMap[*vItStart] = 1;

		int vLen = 1;
		auto vIt = vItStart++;
		while (++vIt != str.cend()) {
			if (vHitMap[*vIt] == 0) {
				vHitMap[*vIt] = 1;
				++vLen;
			} else {
				break;
			}
		}

		if (vLen > vMaxLen) {
			vMaxLen = vLen;
		}
	}

	return vMaxLen;
}

TEST(TSLeetCode, TCLengthOfLongestSubstring)
{
	std::string vStr("abcabcbb");
	EXPECT_EQ(LengthOfLongestSubstring(vStr), 3);

	vStr = "bbbbb";
	EXPECT_EQ(LengthOfLongestSubstring(vStr), 1);

	vStr = "pwwkew";
	EXPECT_EQ(LengthOfLongestSubstring(vStr), 3);

	vStr = "";
	EXPECT_EQ(LengthOfLongestSubstring(vStr), 0);
}

/**
 * My solution on LeetCode:
 * 执行用时：
 * 16 ms, 在所有C++提交中击败了71.08%的用户
 * 内存消耗：
 * 6.8 MB, 在所有C++ 提交中击败了87.69%的用户
 */
class Solution {
public:
	int lengthOfLongestSubstring(string s) {
		constexpr int CHAR_SET_SIZE = 256;
		std::array<char, CHAR_SET_SIZE> vHitMap;
		int vMaxLen = 0;

		auto vItStart = s.cbegin();
		while (vItStart != s.cend()) {
			vHitMap.fill(0);
			vHitMap[*vItStart] = 1;

			int vLen = 1;
			auto vIt = vItStart++;
			while (++vIt != s.cend()) {
				if (vHitMap[*vIt] == 0) {
					vHitMap[*vIt] = 1;
					++vLen;
				} else {
					break;
				}
			}
			if (vLen > vMaxLen) {
				vMaxLen = vLen;
			}
		}

		return vMaxLen;
	}
};

}  // namespace microsoft
}  // namespace leetcode
}  // namespace liuzan
