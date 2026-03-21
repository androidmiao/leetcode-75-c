@[347_Top K Frequent Elements_Description.md](347_Top K Frequent Elements/347_Top K Frequent Elements_Description.md) 請使用Ｃ語言來實作並把結果存到 [347_Top K Frequent Elements.c](347_Top K Frequent Elements/347_Top K Frequent Elements.c) 請詳細說明內容, 逐行中文註解並且畫圖說明

已完成，程式已寫入 [347_Top K Frequent Elements.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/347_Top%20K%20Frequent%20Elements/347_Top%20K%20Frequent%20Elements.c)。

我保留了原本的 `Bucket Sort + Frequency Counting` 解法，並把檔案改成適合自修的版本：檔案頂部有整體思路與 ASCII 圖解，函式內則補上接近逐行的中文註解，從頻率統計、bucket 配置、填入 bucket，到最後從高頻往下取答案都有說明。這個版本可以直接提交到 LeetCode。

我也做了基本編譯檢查：
```bash
gcc -Wall -Wextra -std=c99 -c /Users/alex.miao/Documents/GitHub/leetcode-75-c/347_Top\ K\ Frequent\ Elements/347_Top\ K\ Frequent\ Elements.c -o /tmp/347_top_k_frequent_elements.o
```
結果通過，沒有語法錯誤。

---

@347_Top K Frequent Elements_Editorial.md 跟你寫的解法有一樣嗎？ 如果不一樣, 哪一個比較好？ 並把結果寫進*_GPT-5.4.md

不一樣。

[347_Top K Frequent Elements_Editorial.md](/Users/alex.miao/Documents/GitHub/leetcode-75-c/347_Top%20K%20Frequent%20Elements/347_Top%20K%20Frequent%20Elements_Editorial.md) 主要整理的是官方 editorial 的兩條主線：

1. `Approach 1: Heap`
2. `Approach 2: Quickselect`

但我實際寫進 [347_Top K Frequent Elements.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/347_Top%20K%20Frequent%20Elements/347_Top%20K%20Frequent%20Elements.c) 的是：

- `Bucket Sort + Frequency Counting`

所以結論是：

- **跟 editorial 裡的主解法不一樣**
- **但在這題的限制下，bucket sort 是很合理，甚至我會認為更適合 C 的實作**

### 為什麼不一樣

官方 editorial 這題偏向示範：

- `Heap`：比較直觀，也很常見於面試
- `Quickselect`：平均 `O(n)`，演算法觀念漂亮

但這題其實還有一個非常自然的做法，就是利用：

- 值域固定：`nums[i]` 在 `[-10^4, 10^4]`
- 頻率上限固定：最多只會到 `numsSize`

這讓 `Bucket Sort + Frequency Counting` 變得很好寫。

### 哪一個比較好

如果是這個 repo、這份 C 程式、以及這題本身的限制，我會這樣排：

1. **Bucket Sort**
2. `Heap`
3. `Quickselect`

### 原因

#### 1. Bucket Sort 最適合這題目前條件

我的寫法可以直接利用：

- 固定值域做 frequency counting
- 頻率做 bucket index

這樣可以得到接近線性的時間複雜度，而且邏輯規律、容易驗證、也容易加上教學註解。

對 C 來說，這一點很重要，因為：

- 不需要自己手刻 heap comparator 邏輯
- 不需要實作 partition / pivot 選擇
- 不太容易出現 quickselect 那種細節 bug

#### 2. Heap 是穩健但不是最順手

Heap 的優點是思路清楚，而且 `O(n log k)` 也很好。

但在純 C 裡：

- 你要自己維護 heap
- comparator 與 push/pop 細節比較繁瑣
- 如果只是為了這題，寫起來通常沒有 bucket sort 直觀

所以 heap 是好解法，但不一定是這題在 C 裡最好教、最好寫的版本。

#### 3. Quickselect 理論漂亮，但不一定最適合這份程式

Quickselect 平均時間是 `O(n)`，這點很強。

但缺點也很明顯：

- partition 邏輯比較難寫對
- 重複頻率時要更小心
- 最壞情況仍然會到 `O(n^2)`
- 在教學與自修上，理解成本高於 bucket sort

所以如果目標是：

- 寫出穩定可提交版本
- 用 C 清楚表達
- 保留容易閱讀的逐行中文註解

那我認為 bucket sort 比 quickselect 更合適。

### 最後結論

如果問的是「跟 editorial 一不一樣」：

- **不一樣**

如果問的是「哪個比較好」：

- **在這題目前條件下，我會選我寫的 `Bucket Sort + Frequency Counting`**

原因不是它理論上一定全面勝過 quickselect，而是：

- 它同樣符合 follow-up 的方向
- 在 C 裡更穩、更直觀
- 更適合這個 repo 現在的教學與自修風格
