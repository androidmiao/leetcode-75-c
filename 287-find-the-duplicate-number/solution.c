// https://leetcode.com/problems/find-the-duplicate-number/description/

/*
解題思路：
將陣列視為一個鏈表 (linked list)：把 nums[i] 當作「下一個節點的索引」。
因為陣列長度為 n+1，每個值都落在 [1, n]，必定有重複元素，
這代表「不同的索引」會指向「同一個下一個位置」，形成一個環 (cycle)。
重複的那個數字，就是這個環的入口。

解法分為兩個階段，使用 Floyd 龜兔賽跑（快慢指標）：

階段 1：找出環內的相遇點
  - slow 每次走 1 步：slow = nums[slow]
  - fast 每次走 2 步：fast = nums[nums[fast]]
  - 因為環一定存在，fast 最後一定會在環內追上 slow

階段 2：找出環的入口（即重複數字）
  - 將 fast 重置回起點 nums[0]
  - slow 留在相遇點，兩者改成「每次都只走 1 步」
  - 當 slow 與 fast 再次相遇時，相遇點就是環的入口（重複數字）

數學原理：
  令 a = 起點到環入口的距離
     b = 環入口到相遇點的距離
     c = 相遇點走一圈回到環入口的距離（b + c = 環長）
  階段 1 結束時，slow 走了 a + b 步，fast 走了 2(a + b) 步，
  fast 比 slow 多走了若干圈：2(a + b) - (a + b) = k(b + c)
  化簡得 a = (k - 1)(b + c) + c
  代表「從起點走 a 步」與「從相遇點走 c 步」會同時抵達環入口。
  所以階段 2 兩個指標再次相遇時，必定停在環入口。

範例：nums = [1, 3, 4, 2, 2]
索引: 0   1   2   3   4
值:   1   3   4   2   2

把陣列當鏈表 (從索引 0 出發, 用值當下一步索引):
  0 -> 1 -> 3 -> 2 -> 4 -> 2 -> 4 -> 2 -> ...
                 ^               |
                 └───────────────┘
                 環的入口 = 索引 2 (對應重複數字 2)

階段 1：尋找相遇點
初始：
  slow = nums[0] = 1
  fast = nums[0] = 1

  0 -> [1] -> 3 -> 2 -> 4
       sf

第 1 輪後：
  slow = nums[1] = 3
  fast = nums[nums[1]] = nums[3] = 2

  0 -> 1 -> [3] -> [2] -> 4
            s      f

第 2 輪後：
  slow = nums[3] = 2
  fast = nums[nums[2]] = nums[4] = 2

  0 -> 1 -> 3 -> [2] -> 4
                 sf      ← 相遇！(在環內某點)

階段 2：尋找環的入口
重置 fast 回到起點 nums[0]，slow 留在相遇點 2
  slow = 2, fast = 1
  兩者每次都只走 1 步

第 1 輪後：
  slow = nums[2] = 4
  fast = nums[1] = 3

第 2 輪後：
  slow = nums[4] = 2
  fast = nums[3] = 2          ← 再次相遇！停在重複數字 2

回傳 slow = 2，即為重複數字。

時間複雜度：O(n)
空間複雜度：O(1)
*/

#include "solution.h"

int findDuplicate(int* nums, int n) {
    // slow 指標一次走一步，初始位置在起點 nums[0]
    int slow = nums[0];

    // fast 指標一次走兩步，初始位置也在起點 nums[0]
    int fast = nums[0];

    // 階段 1：尋找環內的相遇點
    // 因為環一定存在，迴圈最終必定終止
    do {
        // slow 前進一步：把目前的值當作下一個索引
        slow = nums[slow];

        // fast 前進兩步：連續做兩次索引跳躍
        fast = nums[nums[fast]];
    } while (slow != fast);
    // 跳出迴圈時，slow 與 fast 在環內某處相遇

    // 階段 2：尋找環的入口（即重複數字所在位置）
    // 將 fast 重置回起點，slow 維持在相遇點
    fast = nums[0];

    // 兩者改為每次都只走一步，再次相遇時即為環入口
    while (slow != fast) {
        // slow 前進一步
        slow = nums[slow];

        // fast 前進一步（與 slow 同步）
        fast = nums[fast];
    }

    // 相遇點就是環的入口，對應的值即為重複的數字
    return slow;
}
