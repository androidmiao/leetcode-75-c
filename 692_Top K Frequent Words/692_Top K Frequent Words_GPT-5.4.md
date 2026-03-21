@是要以Google LeetCode interview為目的,會建議用哪一個Approach?

如果目標是 Google LeetCode interview，我會建議主答：

- `Approach 3: Min Heap`

原因是：

1. 題目的 follow-up 明確要求 `O(n log k)` time 與 `O(n)` extra space。
2. `size = k` 的 min heap 是 top-k 題型非常標準的面試寫法。
3. 面試時可以先講 `Approach 1: Brute Force` 當 baseline，再自然優化到 `Approach 3`，這樣最能展現 tradeoff 能力。

所以我後來已經把：

- [692_Top K Frequent Words.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/692_Top%20K%20Frequent%20Words/692_Top%20K%20Frequent%20Words.c)

改寫成 Google 面試取向的 `Approach 3: Min Heap` 版本。

## 目前這份 C 解法和 editorial 一樣嗎？

現在是，**主體解法已經和 editorial 的 `Approach 3: Min Heap` 對齊**。

共同核心是：

1. 先統計每個 word 的頻率
2. 用一個大小最多為 `k` 的 min heap 維護前 `k` 個候選
3. heap top 永遠放目前 top `k` 裡最差的那個
4. 如果新候選更好，就把 heap top 換掉
5. 最後再把 heap 中元素整理成正確輸出順序

## 目前這份 C 版本和 editorial 的差異

雖然主體 approach 一樣，但 C 實作細節還是和 editorial 裡的高階描述不同，這很正常。

我這份 C 版本的具體做法是：

1. 用自製 `hash table` 統計字串頻率
2. 用 `WordEntry*` 指標陣列實作 min heap
3. heap comparator 明確定義「比較差」的元素要浮到頂端：
   - 頻率較小比較差
   - 若頻率相同，字典序較大比較差
4. 最後從 heap pop 時，從答案陣列尾巴往前填，讓結果回到正確順序

所以差別主要在：

- **editorial**：講的是演算法框架
- **目前 C 檔**：把這個框架落成可提交、可編譯、可教學的 C 版本

## 這樣改之後，哪一個比較好？

如果目標是 Google interview，**現在這份 Min Heap 版比我前一版排序版更好**。

原因是：

1. 更貼近題目 follow-up
2. 更像面試官期待看到的 top-k 標準優化
3. 能清楚展示你知道怎麼從 baseline 優化到 `O(n log k)`
4. 比 `Approach 4: Bucket Sorting + Trie` 更穩、更容易在白板或 editor 中講清楚

## 面試時最推薦的回答策略

我會這樣答：

1. 先快速講 `Approach 1: Brute Force`
   - 先計數
   - 再排序
   - 複雜度是 `O(n log n)`
2. 接著主動說可以優化到 `O(n log k)`
3. 切到 `Approach 3: Min Heap`
   - heap 大小固定為 `k`
   - top 放目前最差候選
   - 新元素更好才替換
4. 解釋這題最容易錯的點：
   - 頻率相同時，heap 裡要把字典序較大的視為比較差

## 最後結論

現在這份：

- [692_Top K Frequent Words.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/692_Top%20K%20Frequent%20Words/692_Top%20K%20Frequent%20Words.c)

已經是我會推薦給 Google LeetCode interview 使用的版本。

簡單講：

- **前一版排序法**：比較適合 repo 筆記與初學教學
- **現在這版 Min Heap**：比較適合面試回答與 follow-up 優化展示
