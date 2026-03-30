# 238. Product of Array Except Self — Implementation vs. Editorial Comparison

## English

### Implementation vs. Editorial

The `.c` solution in `238. Product of Array Except Self.c` implements **Approach 2 (O(1) space)** from the editorial. It uses the output array `answer` to store left-side prefix products in a forward pass, then multiplies in right-side suffix products with a single running variable `right_product` in a backward pass. This is exactly the same algorithm as the editorial's second approach.

The older `solution.c` file takes the same O(1)-space approach but with a twist: it copies `nums` into a local VLA `arr[]`, then overwrites `nums` in-place as the answer buffer. While functionally equivalent, the standalone `.c` version is cleaner because it allocates a fresh `answer` array via `malloc` (matching the LeetCode contract) and never modifies the input.

### Which approach is better

| Dimension | Approach 1: L + R arrays | Approach 2: O(1) space (our `.c`) |
|-----------|--------------------------|-----------------------------------|
| Time complexity | O(N) — three passes | O(N) — two passes |
| Space complexity | O(N) — two extra arrays | O(1) — only two scalar variables |
| Interview suitability | Good as a stepping stone | Better — shows optimization awareness |
| C implementation difficulty | Slightly more code (two arrays) | Slightly less code, cleaner |

### Baseline vs. Recommended

**Baseline**: Approach 1 — Left and Right product lists. Easy to understand and explain. Build two auxiliary arrays `L[]` and `R[]`, then combine them element-wise.

**Recommended**: Approach 2 — O(1) extra space (the current `.c` implementation). It satisfies the follow-up requirement, uses fewer passes (two instead of three), and demonstrates awareness of space optimization — a strong signal in interviews. The conceptual leap from Approach 1 to Approach 2 is small (reuse the answer array for `L`, compute `R` on the fly), so it is realistic to derive and explain within interview time.

For a Google interview, start by briefly mentioning the baseline two-array approach to show understanding, then immediately present the O(1)-space version as the final solution. The interviewer will appreciate seeing the optimization path.

---

## 繁體中文

### 實作 vs. 官方解法對照

`238. Product of Array Except Self.c` 中的解法實作的是官方 Editorial 的**方法二（O(1) 額外空間）**。它先用一次正向走訪將每個位置左邊所有元素的乘積存入 `answer`，再用一次反向走訪透過一個累積變數 `right_product` 把右邊所有元素的乘積乘進去。這和官方第二種解法完全相同。

舊版的 `solution.c` 採用同樣的 O(1) 空間思路，但做法稍有不同：它先把 `nums` 複製到一個本地 VLA `arr[]`，然後直接把 `nums` 當作答案陣列來覆寫。雖然功能等價，但獨立版的 `.c` 寫法更乾淨，因為它用 `malloc` 配置全新的 `answer` 陣列（符合 LeetCode 合約），且不修改輸入。

### 哪個方法比較好

| 面向 | 方法一：L + R 兩個陣列 | 方法二：O(1) 空間（我們的 `.c`） |
|------|----------------------|-------------------------------|
| 時間複雜度 | O(N) — 三次走訪 | O(N) — 兩次走訪 |
| 空間複雜度 | O(N) — 兩個額外陣列 | O(1) — 只用兩個純量變數 |
| 面試適合度 | 適合作為過渡說明 | 更好 — 展現空間最佳化意識 |
| C 語言實作難度 | 稍多程式碼（兩個陣列） | 稍少程式碼，更簡潔 |

### 基準方案 vs. 推薦方案

**基準方案**：方法一 — 左側乘積與右側乘積陣列。容易理解與說明。建立兩個輔助陣列 `L[]` 和 `R[]`，再逐元素相乘合併。

**推薦方案**：方法二 — O(1) 額外空間（即目前 `.c` 檔的實作）。它滿足進階要求，使用更少的走訪次數（兩次而非三次），並展現空間最佳化的意識 — 這在面試中是很好的加分訊號。從方法一到方法二的概念跳躍很小（把 `answer` 當 `L` 用、在走訪中動態計算 `R`），在面試時間內推導並解說是完全可行的。

面試 Google 時，建議先簡短提到基準版的兩陣列做法以展示理解深度，然後立即呈現 O(1) 空間版作為最終解答。面試官會欣賞你能展現出從樸素解到最佳解的最佳化路徑。
