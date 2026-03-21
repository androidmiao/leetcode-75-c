@請把內容翻譯成中文 (保留原來英文內容, 並翻譯成中文放到後面)
- 英文原文 + 對應中文翻譯
- 先整段英文、完整翻譯中文放到後面, 不要中英文交錯
@Description.md 請使用Ｃ語言來實作並把結果存到 @*.c 請詳細說明內容, 逐行中文註解並且畫圖說明
@1768-merge-strings-alternately.c 請畫圖說明並記錄到這個檔案中
@畫「記憶體位址/陣列索引版」的圖
@這個解法跟你的解法哪一個比較好？
@Solution.md 跟你寫的解法有一樣嗎？ 如果不一樣, 哪一個比較好？
@下面限制條件會不會影響目前這個程式設計？ 但如果長度大於100呢?
@OK照你建議請改程式碼
@"有轉型風險", 請說一下範例


---
不完全一樣。

你這份 [283-move-zeroes_Solution.md](/Users/alex.miao/Documents/GitHub/leetcode-75-c/283-move-zeroes/283-move-zeroes_Solution.md) 內有 3 種解法：

1. `Approach #1`：額外開新陣列（`O(n)` 空間）
2. `Approach #2`：原地、兩段式（先收集非 0，再補 0）
3. `Approach #3`：原地 swap，寫入次數更少（最優操作數）

我之前給你的 C 程式是 **Approach #2**，邏輯一致：
- 用 `insertPos` 收集非 0
- 再把後面補 0

所以結論是：**跟文件的第 2 種解法一樣，但不是第 3 種（最少寫入）解法。**


--- TO-DO ---
已改成 `Approach #3`（swap 版）並整理為可直接提交的精簡程式：
diff --git a//Users/alex.miao/Documents/GitHub/leetcode-75-c/283-move-zeroes/283-move-zeroes.c b//Users/alex.miao/Documents/GitHub/leetcode-75-c/283-move-zeroes/283-move-zeroes.c
new file mode 100644
--- /dev/null
+++ b//Users/alex.miao/Documents/GitHub/leetcode-75-c/283-move-zeroes/283-move-zeroes.c
@@ -0,0 +1,15 @@
+// Two Pointers - Approach #3 (Optimal)
+// Time: O(n), Space: O(1)
+
+void moveZeroes(int* nums, int numsSize) {
+    int lastNonZeroFoundAt = 0;
+
+    for (int cur = 0; cur < numsSize; cur++) {
+        if (nums[cur] != 0) {
+            int temp = nums[lastNonZeroFoundAt];
+            nums[lastNonZeroFoundAt] = nums[cur];
+            nums[cur] = temp;
+            lastNonZeroFoundAt++;
+        }
+    }
+}

[283-move-zeroes.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/283-move-zeroes/283-move-zeroes.c)

核心就是：
1. `cur` 掃描整個陣列  
2. 遇到非 0 就和 `lastNonZeroFoundAt` 位置交換  
3. `lastNonZeroFoundAt++`  

這就是題解中的最優做法（原地、`O(n)` 時間、`O(1)` 空間）。