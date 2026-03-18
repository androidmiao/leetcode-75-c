已把 C 語言實作寫入 [239_Sliding Window Maximum.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/239_Sliding Window Maximum/239_Sliding Window Maximum.c)。

這份解法使用單調遞減 deque，核心觀念是讓佇列內只保留「可能成為目前或未來視窗最大值」的索引，因此每個元素最多進出一次，時間複雜度是 O(n)，符合這題標準最佳解。檔案內也已補上逐行中文註解，方便你直接對照每一段邏輯閱讀。




已把圖解說明直接寫進 [239_Sliding Window Maximum.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/239_Sliding Window Maximum/239_Sliding Window Maximum.c)。

我加的是程式前面的大段中文註解，內容包含：

deque 為什麼存索引
為什麼要維持單調遞減
以 nums = [1,3,-1,-3,5,3,6,7], k = 3 逐步畫出每一步的 deque(索引) 與 deque(數值) 變化
為什麼隊尾較小元素可以刪掉
程式本體沒有改邏輯，所以仍然是可提交的 O(n) 解法。若你要，我可以下一步再幫你改成你之前那種「記憶體位址 / 陣列索引版」ASCII 圖。





不完全一樣，但核心解法是同一種。

[239_Sliding Window Maximum_Solution.md](/Users/alex.miao/Documents/GitHub/leetcode-75-c/239_Sliding Window Maximum/239_Sliding Window Maximum_Solution.md#L6) 用的是 `Monotonic Deque`，我的 [239_Sliding Window Maximum.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/239_Sliding Window Maximum/239_Sliding Window Maximum.c#L3) 也是單調遞減 `deque`。兩者的核心規則都一樣：
- `deque` 存索引，不存值
- 隊首保留目前視窗最大值
- 新元素進來前，先把隊尾比較小的元素移掉
- 視窗左邊滑出範圍的索引，要從隊首移掉

差異主要有兩個。

第一，[Solution.md](/Users/alex.miao/Documents/GitHub/leetcode-75-c/239_Sliding Window Maximum/239_Sliding Window Maximum_Solution.md#L48) 把流程拆成兩段：
1. 先處理前 `k` 個元素，建立第一個視窗
2. 再從 `i = k` 開始處理後續視窗

我的 [C 實作](/Users/alex.miao/Documents/GitHub/leetcode-75-c/239_Sliding Window Maximum/239_Sliding Window Maximum.c#L110) 是單一 `for` 迴圈一路跑到底，當 `i >= k - 1` 才開始輸出答案。這只是寫法不同，不是演算法不同。

第二，空間上有實作差異。[Solution.md](/Users/alex.miao/Documents/GitHub/leetcode-75-c/239_Sliding Window Maximum/239_Sliding Window Maximum_Solution.md#L145) 寫的是 `O(k)`，這是理論上 deque 真正需要的大小；但我的程式在 [這行](/Users/alex.miao/Documents/GitHub/leetcode-75-c/239_Sliding Window Maximum/239_Sliding Window Maximum.c#L101) 配了 `numsSize` 大小的陣列給 `deque`，所以我這份程式的實際額外配置是 `O(n)`。

所以結論是：
- 如果看「演算法思想」，兩個一樣，都是標準最佳解。
- 如果看「程式實作品質」，`Solution.md` 的空間複雜度版本更好，因為它的目標是 `O(k)`。
- 如果看「程式可讀性」，我現在這份 C 寫法比較直觀，單一迴圈也比較好讀。

嚴格來說，較好的版本是：保留我現在這份 C 的單迴圈寫法，但把 `deque` 改成只用 `k` 大小的環形佇列，這樣就同時有可讀性和 `O(k)` 空間。你要的話，我可以直接幫你把 [239_Sliding Window Maximum.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/239_Sliding Window Maximum/239_Sliding Window Maximum.c) 改成這個版本。




已把 [239_Sliding Window Maximum.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/239_Sliding Window Maximum/239_Sliding Window Maximum.c) 改成 O(k) 空间版本。

现在核心演算法还是单调递减 deque，但辅助结构改成只配置 k 个位置的环形数组，不再用 numsSize 大小的 deque。我也同步更新了档案里的中文注解与复杂度说明，所以现在它和 [239_Sliding Window Maximum_Solution.md](/Users/alex.miao/Documents/GitHub/leetcode-75-c/239_Sliding Window Maximum/239_Sliding Window Maximum_Solution.md) 在演算法层面与空间复杂度上是一致的。

已验证：

cc -fsyntax-only 通过，没有语法错误。