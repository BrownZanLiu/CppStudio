/**
给你两个非空的链表，表示两个非负的整数。它们每位数字都是按照逆序的方式存储的，并且每个节点只能存储一位数字。
请你将两个数相加，并以相同形式返回一个表示和的链表。
你可以假设除了数字 0 之外，这两个数都不会以 0 开头

示例 1：
输入：l1 = [2,4,3], l2 = [5,6,4]
输出：[7,0,8]
解释：342 + 465 = 807.

示例 2：
输入：l1 = [0], l2 = [0]
输出：[0]

示例 3：
输入：l1 = [9,9,9,9,9,9,9], l2 = [9,9,9,9]
输出：[8,9,9,9,0,0,0,1]

提示：
每个链表中的节点数在范围 [1, 100] 内
0 <= Node.val <= 9
题目数据保证列表表示的数字不含前导零

来源：力扣（LeetCode）
链接：https://leetcode.cn/problems/add-two-numbers
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。
*/

/**
 * Copyright: brown.liuzan@outlook.com
 */

#include <iterator>
#include <forward_list>

#include <gtest/gtest.h>

namespace liuzan {
namespace leetcode {
namespace microsoft {

auto AddTwoNumbers(std::forward_list<int> const &a, std::forward_list<int> const &b)
{
	std::forward_list<int> vSum;
	int vDigitSum = 0;
	int vCarried = 0;
	auto vItA = a.cbegin();
	auto vItB = b.cbegin();
	auto vItS = vSum.before_begin();
	while (vItA != a.cend() || vItB != b.cend()) {
		vDigitSum += vCarried;
		vCarried = 0;
		if (vItA != a.cend()) {
			vDigitSum += *vItA;
			vItA = std::next(vItA);
		}
		if (vItB != b.cend()) {
			vDigitSum += *vItB;
			vItB = std::next(vItB);
		}
		vItS = vSum.insert_after(vItS, vDigitSum % 10);
		if (vDigitSum / 10) {
			vCarried = 1;
		}
		vDigitSum = 0;
	}
	if (vCarried) {
		vSum.insert_after(vItS, 1);
	}

	return std::move(vSum);
}

TEST(TSLeetCodeMicrosoft, TCAddTwoNumbers)
{
	auto a = std::forward_list<int>{2, 4, 3};
	auto b = std::forward_list<int>{5, 6, 4};
	auto s = AddTwoNumbers(a, b);
	auto t = std::forward_list<int>{7, 0, 8};
	EXPECT_EQ(s, t);

	a = std::forward_list<int>{0};
	b = std::forward_list<int>{0};
	s = AddTwoNumbers(a, b);
	t = std::forward_list<int>{0};
	EXPECT_EQ(s, t);

	a = std::forward_list<int>{9, 9, 9, 9, 9, 9, 9};
	b = std::forward_list<int>{9, 9, 9, 9};
	s = AddTwoNumbers(a, b);
	t = std::forward_list<int>{8, 9, 9, 9, 0, 0, 0, 1};
	EXPECT_EQ(s, t);
}

/**
 * My solution on Leetcode.
 * Date: 2023-06-27
 * Speed performance: 8ms; better than 99.89%
 * Memory usage: 69.6MB; better than 76%
 */
struct ListNode {
	int val;
	ListNode *next;
	ListNode() : val(0), next(nullptr) {}
	ListNode(int x) : val(x), next(nullptr) {}
	ListNode(int x, ListNode *next) : val(x), next(next) {}
};

class Solution {
public:
	ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
		ListNode *vpResult = new ListNode();
		ListNode *vpCur = vpResult;
		ListNode *vpPrev = nullptr;
		ListNode *vpA = l1;
		ListNode *vpB = l2;
		int vCarried = 0;
		int vSum = 0;

		while (vpA != nullptr || vpB != nullptr) {
			vSum += vCarried;
			vCarried = 0;
			if (vpA != nullptr) {
				vSum += vpA->val;
				vpA = vpA->next;
			}
			if (vpB != nullptr) {
				vSum += vpB->val;
				vpB = vpB->next;
			}
			vCarried = vSum / 10;
			vpCur->val = vSum % 10;
			vpCur->next = new ListNode(vSum % 10);
			vpPrev = vpCur;
			vpCur = vpCur->next;
			vSum = 0;
		}

		if (vCarried) {
			vpCur->val = 1;
		} else if (vpPrev != nullptr) {
			vpPrev->next = nullptr;
			delete vpCur;
		}

		return vpResult;
	}
};

}  // namespace microsoft
}  // namesapce leetcode
}  // namespace liuzan
