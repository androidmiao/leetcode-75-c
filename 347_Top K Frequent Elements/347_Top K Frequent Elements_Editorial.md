# 347. Top K Frequent Elements Editorial

This note is a high-fidelity rewrite of the official LeetCode editorial. It follows the same section flow and covers the same core ideas, while rewriting the wording into repo-friendly bilingual study notes.

---

## English Version

### Solution Article Overview

The official article presents two main approaches for this problem. The first relies on a heap and is intended as the more straightforward practical solution. The second uses quickselect, which is more algorithmically advanced and achieves linear average time. The article then ends with a short theory discussion about whether one could force worst-case linear time.

### Approach 1: Heap

#### Intuition

The heap-based approach starts from the observation that we do not need a full ordering of every distinct value. We only care about the top `k` most frequent elements, so it is enough to keep a data structure that always remembers the best `k` candidates seen so far.

#### Main idea

First count frequencies with a hash map. Then iterate through all distinct values and maintain a heap whose size never exceeds `k`. Whenever a new candidate is inserted and the heap becomes too large, remove the least useful one. At the end, the heap contains exactly the values we need.

#### Step-by-step outline

1. Build a frequency table `value -> count`.
2. Use the distinct values as heap candidates.
3. Keep the heap size at most `k`.
4. After scanning the map, convert the heap contents into the output array.

#### Why it works

The heap is always trimmed to `k` elements, so any value left inside it must be competitive among the highest frequencies encountered so far. After the scan finishes, no excluded value can outrank all values inside the heap, so the remaining set is the correct top `k`.

#### Complexity Analysis

The counting phase takes `O(N)`. Maintaining the heap across the distinct values costs about `O(N log k)` in the broad problem-scale view, and the extra memory is `O(N + k)`.

#### Practical note

This is the easier of the two editorial approaches to implement correctly. It is especially attractive when `k` is much smaller than the number of distinct values.

### Approach 2: Quickselect (Hoare's selection algorithm)

#### Intuition

Quickselect is designed for questions of the form "find the `k`th something". In this problem, the ordering key is frequency rather than numeric value, so the target becomes: arrange the distinct values so that the split at index `N - k` separates lower-frequency elements from the top `k` frequent ones.

#### Core idea

The algorithm mirrors quicksort at a high level. Pick a pivot, partition the array around that pivot according to frequency, and place the pivot into the position it would occupy in a fully frequency-sorted array. The key difference from quicksort is that we only continue in the side that can still contain the target split index.

#### Frequency-based view

Suppose we store every distinct value in an array called `unique`. After partitioning around some pivot frequency, values on the left side are less frequent, while values on the right side are more frequent or equally frequent. If the pivot ends up exactly at `N - k`, then every value to its right belongs to the final answer set.

#### Algorithm

1. Build a frequency table.
2. Collect all distinct values into `unique`.
3. Choose a pivot in the current range.
4. Partition the range according to pivot frequency.
5. Compare the pivot's final index with `N - k`.
6. If they match, stop.
7. Otherwise, recurse or iterate only into the side that still contains the target index.

#### Why quickselect is faster on average

If we used full quicksort logic, we would have to process both sides after every partition, which would lead to the familiar `O(N log N)` average cost. Quickselect avoids that extra work because once the pivot tells us which side contains the answer boundary, the other side can be ignored.

#### Partition detail: Lomuto scheme

The editorial explains the concrete implementation with Lomuto partitioning. Move the pivot to the end of the current range, keep a pointer such as `store_index`, and scan the range once. Every time you meet an element whose frequency is lower than the pivot's frequency, swap it toward the front and advance `store_index`. Finally, swap the pivot back into `store_index`.

#### Why duplicates matter

The values in `unique` are distinct, but their frequencies are not guaranteed to be distinct. Because of that, the partition rule must remain correct even when several elements share the same frequency. This is one reason the editorial explicitly discusses the partition scheme.

#### Complexity Analysis

The average running time is `O(N)`, which is the main attraction of this method. The worst case is still `O(N^2)` if pivots keep splitting the array very poorly. The extra space is up to `O(N)` because we still store the frequency map and the array of distinct values.

#### Practical note

Quickselect is algorithmically elegant, but the implementation burden is noticeably higher than the heap solution. It is valuable when you want a stronger asymptotic average bound and are confident about partition-based logic.

### Further Discussion: Could We Do Worst-Case Linear Time?

The editorial briefly mentions Median of Medians as the classic theoretical answer to worst-case linear-time selection. Even though that method has the right asymptotic guarantee, it is rarely preferred in practice because the constants are high and the implementation is much less attractive.

#### Editorial conclusion

In other words, the official article presents heap as the simpler practical route and quickselect as the more advanced average-linear route, while acknowledging that theoretical worst-case linear selection is usually not the practical answer.

### Editorial-to-Repo Mapping

The current C solution in this folder does not have to match the official article one-to-one. A bucket-sort-style implementation is still a perfectly valid repo choice because the problem's follow-up strongly encourages near-linear solutions, and bucket grouping is often easier to implement cleanly in C.

---

## 中文翻譯

### 解題文章總覽

官方文章主要介紹兩種方法。第一種是 heap，重點在於實作相對直接、面試上也很實用；第二種是 quickselect，演算法上更進一步，平均時間可以做到線性。文章最後再補一段理論討論，說明是否能進一步保證 worst-case 線性時間。

### Approach 1: Heap

#### 直覺

Heap 方法的出發點是：我們其實不需要把所有不同元素完整排序。因為題目只要求前 `k` 個高頻元素，所以只要維護一個能夠持續保留目前最佳 `k` 個候選值的資料結構就夠了。

#### 核心想法

流程是先用 hash map 統計頻率，接著走訪所有不同元素，同時維護一個大小不超過 `k` 的 heap。每當加入新候選值後 heap 超過限制，就刪掉目前最不值得保留的那個元素。當所有元素都處理完後，heap 中剩下的就是答案。

#### 步驟整理

1. 先建立 `value -> count` 的頻率表。
2. 將所有不同元素當作 heap 候選值。
3. 讓 heap 的大小始終不超過 `k`。
4. 掃描完成後，把 heap 裡的元素整理成輸出陣列。

#### 為什麼成立

因為 heap 會一直被限制在 `k` 個元素，所以最後留在 heap 裡的值，必然都是目前頻率最有競爭力的候選者。當掃描結束時，被排除的元素不可能再超越 heap 內所有元素，因此 heap 中保留下來的集合就是正確答案。

#### 複雜度分析

計數階段需要 `O(N)`。之後用 heap 維護前 `k` 個高頻元素，整體可視為大約 `O(N log k)`。額外空間則是 `O(N + k)`。

#### 實務觀察

這是官方文章中較容易正確實作的一種方法。特別是在 `k` 明顯小於不同元素個數時，這個做法通常非常合適。

### Approach 2: Quickselect (Hoare's selection algorithm)

#### 直覺

Quickselect 原本就是為了處理「找第 `k` 個某種東西」的問題而設計。在這題裡，排序依據不是數值大小，而是出現頻率，因此目標變成：把不同元素重新排列，讓 `N - k` 這個切點左邊是較低頻元素，右邊則是 top `k` 高頻元素。

#### 核心概念

它的整體概念和 quicksort 很像：先挑一個 pivot，依照頻率把元素分區，然後把 pivot 放到它在完整頻率排序中應該出現的位置。和 quicksort 最大的差別在於，quickselect 不需要同時處理兩側，只要繼續追蹤仍可能包含目標切點的那一邊即可。

#### 從頻率角度理解

假設我們把所有不同元素放進一個 `unique` 陣列。當我們依照 pivot 的頻率完成 partition 後，左側都會是較低頻元素，右側則是較高頻或相同頻率的元素。如果 pivot 最後剛好落在 `N - k` 位置，那麼 pivot 右側的元素就已經構成答案區間。

#### 步驟整理

1. 先建立頻率表。
2. 把所有不同元素收集到 `unique` 陣列。
3. 在目前處理區間中選一個 pivot。
4. 根據 pivot 的頻率進行 partition。
5. 比較 pivot 最終位置與 `N - k`。
6. 如果兩者相同，就可以停止。
7. 如果不同，就只進入仍可能包含目標位置的那一邊。

#### 為什麼平均更快

如果沿用完整 quicksort 的邏輯，每次 partition 後都必須繼續處理左右兩側，平均時間就會回到 `O(N log N)`。Quickselect 的優勢在於，一旦 pivot 告訴我們答案邊界在哪一側，另一側就可以直接忽略，因此平均成本會更低。

#### Partition 細節：Lomuto scheme

官方文章在實作細節上使用的是 Lomuto partition。做法是先把 pivot 移到目前區間尾端，準備一個像 `store_index` 這樣的指標，再用一次線性掃描處理整段區間。每當遇到頻率低於 pivot 的元素，就把它換到前面，並讓 `store_index` 往右移。掃描結束後，再把 pivot 換回 `store_index` 所在的位置。

#### 為什麼要注意重複頻率

雖然 `unique` 陣列中的元素彼此不同，但它們的頻率不一定不同。也就是說，可能有多個值共享同樣的出現次數。因此 partition 規則必須在有重複頻率時依然正確，這也是為什麼官方文章會特別說明 partition 實作。

#### 複雜度分析

這個方法最吸引人的地方是平均時間複雜度可達 `O(N)`。不過如果 pivot 一直選得很差，最壞情況仍然會退化成 `O(N^2)`。額外空間最多約 `O(N)`，主要來自頻率表與不同元素陣列。

#### 實務觀察

Quickselect 在演算法觀點上很漂亮，但實作難度明顯高於 heap 解法。若你希望取得更強的平均複雜度，且對 partition 類演算法有足夠把握，這會是很值得掌握的方法。

### 延伸討論：能不能做到 worst-case 線性時間？

官方最後提到，若從理論上追求 worst-case 線性時間，可以考慮 Median of Medians。雖然它在漸進複雜度上很漂亮，但實務上通常不受歡迎，因為常數大、實作也複雜得多。

#### 文章結論

換句話說，官方文章把 heap 視為較務實、較容易落地的方法，把 quickselect 視為更進階、平均線性的方案；至於理論上的 worst-case 線性選擇法，則多半只是在概念上值得知道。

### 與目前 repo 解法的對照

這個資料夾裡的 C 解法不一定要和官方 article 一一對應。以 bucket-sort 思路完成實作仍然是很合理的 repo 選擇，因為題目的 follow-up 本來就鼓勵接近線性的解法，而 bucket 分組在 C 裡通常也更容易寫得穩定清楚。
