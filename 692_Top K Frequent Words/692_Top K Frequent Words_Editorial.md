# 692. Top K Frequent Words Editorial

This note is a high-fidelity rewrite of the live LeetCode editorial's Solution section, arranged as bilingual study material for this repo.

---

## English Version

### Solution

### Overview

At first glance, this problem is very close to `347. Top K Frequent Elements`, but the editorial highlights three differences that make this version trickier:

1. the input contains strings instead of integers
2. the answer must already be returned in sorted order, not just as an unordered top-`k` set
3. frequencies are not guaranteed to be unique, so ties must be broken lexicographically

Because of that, the main difficulty is no longer frequency counting itself. The harder part is choosing a data structure that can both respect frequency and also return lexicographically smaller words first when frequencies tie.

The editorial says the detailed frequency-selection ideas already appear in problem `347`, so this article gives shorter versions of those approaches and focuses on how to adapt them to string ordering.

### Approach 1: Brute Force

#### Intuition

If we want the top `k` most frequent words, the most direct idea is to sort the whole candidate set by the exact ranking rule required by the problem:

- higher frequency first
- when frequencies tie, lexicographically smaller word first

After that, the first `k` words are the answer.

#### Algorithm

1. Count the frequency of every word in a hash map `cnt`.
2. Collect all words that appear in the input.
3. Sort the full candidate list by:
   - frequency from high to low
   - lexicographical order from smaller to larger when frequencies are equal
4. Return only the first `k` words.

#### Complexity Analysis

Let `N` be the length of `words`.

- Time Complexity: `O(N log N)`
- Space Complexity: `O(N)`

The editorial explains this as `O(N)` counting plus `O(N log N)` sorting, with `O(N)` extra space for the counter and returned slice.

### Approach 2: Max Heap

#### Intuition

Instead of sorting all candidates completely, we can place them into a max heap. Then the top of the heap is always the best remaining candidate according to the ranking rule.

That means we can repeatedly pop the heap `k` times. Each pop gives the next correct word in output order.

#### Algorithm

1. Count each word's frequency just like in Approach 1.
2. Heapify the words into a max heap `h`.
3. The heap order should make the root be:
   - the word with the largest frequency
   - among equal frequencies, the lexicographically smaller word
4. Pop the heap `k` times.
5. The popped words already form the result in correct order.

#### Complexity Analysis

Let `N` be the length of `words`.

- Time Complexity: `O(N + k log N)`
- Space Complexity: `O(N)`

The editorial describes this as `O(N)` counting, `O(N)` heapify, and then `k` heap pops at `O(log N)` each.

### Approach 3: Min Heap

#### Intuition

The max-heap method works well for a fixed offline input, but many top-`k` problems are better handled by maintaining a min heap of size `k`. This lets us keep only the current best `k` candidates seen so far.

Whenever a new word is considered, compare it against the heap top. If it deserves to be in the top `k`, keep it; otherwise discard it.

The subtle point is ordering. We are no longer comparing only frequencies. We must also handle lexicographical tie-breaking correctly.

#### Algorithm

This follows the same high-level idea as the heap solution for problem `347`, with two extra care points:

1. The heap comparator must consider both frequency and lexicographical order.
2. A min heap does not directly store elements in final output order, so after heap maintenance finishes, we must either:
   - sort the heap contents before returning, or
   - pop elements one by one into the result in the correct order

The workflow is:

1. Count word frequencies.
2. Iterate through the candidate words.
3. Push each candidate into a min heap of size at most `k`.
4. If the heap grows too large, remove the current worst candidate.
5. Reconstruct the result from the heap in sorted output order.

#### Complexity Analysis

Let `N` be the length of `words`.

- Time Complexity: `O(N log k)`
- Space Complexity: `O(N)`

The editorial breaks this down as `O(N)` counting, `O(N log k)` heap maintenance, and up to `O(k log k)` to finalize the heap contents. Since `k <= N`, the overall bound remains `O(N log k)`.

### Approach 4: Bucket Sorting + Trie

#### Intuition

If we want to beat comparison-based sorting, we need to avoid paying the usual `k log k` sorting cost on the selected results. The editorial uses counting-style bucket sorting to group words by frequency, then uses a trie inside each bucket so that words with the same frequency can be read back in lexicographical order.

The key idea is:

- use bucket index `i` to represent frequency `i`
- store all words with the same frequency in the same trie
- traverse the buckets from high frequency to low frequency
- inside a bucket, traverse the trie in lexicographical DFS order

Because trie traversal naturally returns strings in dictionary order, the tie-breaking rule is handled without doing an extra comparison sort.

The editorial gives the example:

`words = ["ab", "ed", "ed", "ed", "ac"]`

Here:

- `"ed"` appears 3 times, so it goes into bucket 3
- `"ab"` and `"ac"` appear 1 time, so both go into bucket 1

If we scan buckets from high to low and traverse each trie lexicographically, we obtain:

`["ed", "ab", "ac"]`

#### Algorithm

1. Count the frequency of each word in `cnt`.
2. Create a bucket array of length `N + 1`.
3. Each `bucket[i]` stores a trie that contains all words with frequency `i`.
4. Insert every word from `cnt` into the trie that corresponds to its frequency.
5. Iterate frequency from `N` down to `1`.
6. For each non-empty trie, traverse it in lexicographical DFS order and append words to the answer.
7. Stop once `k` words have been collected.

#### Complexity Analysis

Let `N` be the length of `words`.

- Time Complexity: `O(N)`
- Space Complexity: `O(N)`

The editorial justifies the linear bound by combining:

- `O(N)` for counting
- `O(N)` for distributing words into tries
- `O(N)` for scanning buckets
- `O(k)` to emit the selected words from trie paths, where each word length is bounded and negligible here

It also notes a practical warning: even though the asymptotic time is `O(N)`, the constant factors can be large, so this method may still run slower than simpler approaches.

---

## 中文翻譯

### Solution

### 總覽

乍看之下，這題和 `347. Top K Frequent Elements` 非常接近，但官方 editorial 特別指出這題有三個更麻煩的地方：

1. 輸入是字串，不是整數
2. 回傳結果必須本身就排好序，不能只是任意順序的 top `k`
3. 頻率不一定唯一，所以頻率相同時還要再用字典序做 tie-break

因此，真正的難點不在於「怎麼計數」，而在於要選哪種資料結構，才能同時處理頻率排序與頻率相同時字典序較小者優先。

官方 editorial 表示，關於「如何找出 top `k` 頻率元素」的大方向，在 `347` 已經講過，所以這題主要是把那些方法縮寫後搬過來，重點放在如何改成適用字串排序。

### Approach 1: Brute Force

#### 直覺

既然題目要的是前 `k` 個最高頻單字，最直白的方法就是直接依照題目要求的最終排名規則把所有候選排序：

- 頻率高的排前面
- 頻率相同時，字典序較小的排前面

排序完之後，前 `k` 個就是答案。

#### 演算法

1. 用 hash map `cnt` 統計每個單字出現次數。
2. 收集所有出現在輸入中的單字。
3. 依下列規則排序整個候選清單：
   - 頻率由高到低
   - 如果頻率相同，字典序由小到大
4. 只回傳前 `k` 個單字。

#### 複雜度分析

令 `N` 為 `words` 的長度。

- 時間複雜度：`O(N log N)`
- 空間複雜度：`O(N)`

官方把它拆成 `O(N)` 的計數，加上 `O(N log N)` 的排序，額外空間則來自 counter 和回傳切片。

### Approach 2: Max Heap

#### 直覺

另一個方法是不完整排序所有候選，而是把它們放進 max heap。這樣 heap 頂端永遠是目前最好的候選。

因此我們只要連續從 heap 取出 `k` 次，每次取到的就是下一個正確答案，順序也已經是對的。

#### 演算法

1. 和 Approach 1 一樣，先統計每個單字的頻率。
2. 把所有單字 heapify 成一個 max heap `h`。
3. 這個 heap 的順序要保證根節點是：
   - 頻率最大的單字
   - 如果頻率相同，字典序較小的單字
4. 從 heap 中彈出 `k` 次。
5. 取出的單字就是正確順序的答案。

#### 複雜度分析

令 `N` 為 `words` 的長度。

- 時間複雜度：`O(N + k log N)`
- 空間複雜度：`O(N)`

官方的拆法是：`O(N)` 計數，`O(N)` 建 heap，再做 `k` 次 `O(log N)` 的 pop。

### Approach 3: Min Heap

#### 直覺

max heap 很適合一次性的離線輸入；但很多 top-`k` 題目更常見的做法，是維護一個大小為 `k` 的 min heap，只保留目前最好的 `k` 個候選。

每看到一個新單字，就拿它和 heap 頂端比較。如果它有資格進前 `k`，就保留；否則就丟掉。

這題的微妙點在於 comparator 不能只看頻率，還要同時處理字典序。

#### 演算法

這個方法和 `347` 的 heap 解法主軸相同，但有兩個額外注意點：

1. heap comparator 必須同時考慮頻率與字典序。
2. min heap 內部不保證最後輸出順序，所以維護完 heap 後，還要再：
   - 先把 heap 內容排序後回傳，或
   - 依序把元素彈出並重建成正確答案順序

流程如下：

1. 統計單字頻率。
2. 逐一走訪所有候選單字。
3. 把每個候選放進大小最多為 `k` 的 min heap。
4. 如果 heap 太大，就移除目前最差的候選。
5. 最後再把 heap 內容整理成題目要求的順序。

#### 複雜度分析

令 `N` 為 `words` 的長度。

- 時間複雜度：`O(N log k)`
- 空間複雜度：`O(N)`

官方把它拆成 `O(N)` 計數、`O(N log k)` 的 heap 維護，以及最多 `O(k log k)` 的最後整理；因為 `k <= N`，總體仍是 `O(N log k)`。

### Approach 4: Bucket Sorting + Trie

#### 直覺

如果想超越 comparison sort，就不能再為了輸出結果付出典型的 `k log k` 排序代價。官方這裡用 counting-style 的 bucket sorting 先依頻率分組，再在每個 bucket 裡用 trie 儲存單字，讓同頻率的字能直接以字典序輸出。

核心概念是：

- 用 bucket 索引 `i` 表示頻率 `i`
- 所有頻率相同的字都放進同一個 trie
- 由高頻往低頻掃 bucket
- 在 bucket 內，用 trie 的 DFS 字典序走訪取字

因為 trie 天然就能按字典序走訪，所以頻率相同時的 tie-break 不需要再額外比較排序。

官方 editorial 給的例子是：

`words = ["ab", "ed", "ed", "ed", "ac"]`

在這個例子裡：

- `"ed"` 出現 3 次，所以放進 bucket 3
- `"ab"` 和 `"ac"` 各出現 1 次，所以都放進 bucket 1

如果由高頻掃到低頻，再對每個 trie 做字典序 DFS，最後會得到：

`["ed", "ab", "ac"]`

#### 演算法

1. 用 `cnt` 統計每個單字的頻率。
2. 建立長度為 `N + 1` 的 bucket 陣列。
3. 每個 `bucket[i]` 都是一棵 trie，用來存放頻率等於 `i` 的所有單字。
4. 把 `cnt` 中每個單字插入對應頻率的 trie。
5. 從 `N` 一路往下枚舉到 `1`。
6. 對每個非空 trie 做字典序 DFS，依序把單字加入答案。
7. 一旦收集到 `k` 個單字就停止。

#### 複雜度分析

令 `N` 為 `words` 的長度。

- 時間複雜度：`O(N)`
- 空間複雜度：`O(N)`

官方的理由是：

- `O(N)` 做頻率統計
- `O(N)` 把單字放進 tries
- `O(N)` 掃 bucket
- `O(k)` 從 trie 中產生結果字串，而本題單字長度上限很小，可以視為常數級

官方也特別補充了一個實務提醒：雖然漸進時間複雜度是 `O(N)`，但常數因子很大，因此實際跑起來不一定會比前面較簡單的方法更快。
