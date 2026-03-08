不完全一樣，但**核心思路是一樣的**。

[933_Number of Recent Calls_Solution.md](/Users/alex.miao/Documents/GitHub/leetcode-75-c/933_Number of Recent Calls/933_Number of Recent Calls_Solution.md) 的解法是：

- 用 **sliding window**
- 每次 `ping(t)` 先加入新時間
- 再把所有 `< t - 3000` 的舊資料移掉
- 最後回傳目前視窗內的元素數量

我寫的 [933_Number of Recent Calls.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/933_Number of Recent Calls/933_Number of Recent Calls.c) 也是這個邏輯，所以在**演算法層面是相同的**。

差別在資料結構實作：

- `Solution.md` 偏向用 `LinkedList` / `deque`
- 我寫的 C 版本是用 **固定大小陣列 + `front/rear`** 來模擬 queue

所以結論是：

- **想法一樣**
- **實作方式不同**
- 時間複雜度觀念也一致，都是把過期資料從視窗前端移除，再計算目前視窗大小

---

| 比較項目 | 一樣 | 不一樣 |
|---|---|---|
| 核心觀念 | 都是 **Sliding Window / Queue** 的想法 | 無 |
| `ping(t)` 的主要流程 | 都是「先加入新請求，再移除過期請求，最後回傳目前數量」 | 無 |
| 過期條件 | 都是移除 `< t - 3000` 的資料，保留 `[t - 3000, t]` | 無 |
| 利用題目條件 | 都有利用 `t` 會嚴格遞增這個條件 | 無 |
| 最後答案怎麼算 | 都是看目前視窗內還剩幾個元素 | 無 |
| 演算法本質 | 都是在維護「最近 3000 毫秒內的請求集合」 | 無 |
| 使用的資料結構 | 都可視為在用 queue 觀念 | `Solution.md` 偏向 `LinkedList` / `deque`；C 程式用 **固定陣列 + front/rear** 模擬 queue |
| 實作語言風格 | 都能達成同樣功能 | `Solution.md` 是觀念型說明；`.c` 是 C 語言的具體實作 |
| 記憶體配置方式 | 都需要儲存目前視窗內的請求 | `Solution.md` 的 linked list / deque 通常可動態擴充；C 版直接開 `data[10000]` |
| 空間複雜度寫法 | 都是在維護有限大小的視窗 | `Solution.md` 寫成 `O(1)`，因為視窗上限視為常數；C 版若用一般程式分析，也常會說是 `O(n)`，但在這題限制下也可視為常數上限 |
| 程式細節 | 都有 front/head 與 rear/tail 的概念 | C 版需要自己管理索引、`malloc/free`；題解文字版不強調這些底層細節 |

**結論**

- **一樣的地方**：演算法思想幾乎一樣，都是滑動視窗。
- **不一樣的地方**：主要差在資料結構的實作形式，`Solution.md` 用 `LinkedList/deque` 來講，C 版用陣列索引來做。

---

## 陣列版 vs linked-list 版（C 程式對照）

下面這份對照是比較：

- [933_Number of Recent Calls.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/933_Number of Recent Calls/933_Number of Recent Calls.c)
- [933_Number of Recent Calls_linked-list.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/933_Number of Recent Calls/933_Number of Recent Calls_linked-list.c)

| 比較項目 | 陣列版 | linked-list 版 |
|---|---|---|
| 使用資料結構 | 固定大小陣列 `data[10000]` | 單向 linked list |
| queue 前端 | 用 `front` 索引表示 | 用 `head` 指標表示 |
| queue 尾端 | 用 `rear` 索引表示 | 用 `tail` 指標表示 |
| 新元素加入方式 | `data[rear++] = t` | 建立新節點後接到 `tail` 後面 |
| 移除過期元素方式 | `front++`，不真的刪資料 | 移動 `head`，並 `free()` 舊節點 |
| 回傳目前數量 | `rear - front` | 直接回傳 `size` |
| 記憶體配置 | 幾乎只在 `create()` 時配置一次 | 每次 `ping()` 都會 `malloc()` 一個新節點 |
| 記憶體釋放 | `free(obj)` 即可 | 要逐個 `free` 節點，最後再 `free(obj)` |
| 實作複雜度 | 較低，程式比較短 | 較高，要處理節點串接與釋放 |
| 常數成本 | 通常較低 | 通常較高，因為有動態配置與指標操作 |
| 彈性 | 受限於陣列大小 | 結構較彈性，不依賴固定容量寫法 |

### 一樣的地方

- 兩者都是 sliding window / queue 的解法。
- 兩者都利用 `t` 會嚴格遞增，所以只需要從前端移除過期資料。
- 兩者都保留區間 `[t - 3000, t]` 內的請求。
- 兩者的核心流程都一樣：加入新請求 -> 移除過期請求 -> 回傳剩餘數量。

### 不一樣的地方

- 陣列版是「用索引管理區間」，linked-list 版是「用節點串接 queue」。
- 陣列版移除元素時只是把 `front` 往後移，linked-list 版則真的把前端節點釋放掉。
- 陣列版比較貼近 LeetCode 這題的限制條件，linked-list 版比較貼近 `Solution.md` 所描述的 `LinkedList / deque` 思路。
- linked-list 版的記憶體管理責任比較重，必須處理 `malloc()` / `free()`；陣列版則單純很多。

### 哪個比較適合這題？

- **如果目標是 LeetCode 這題寫得簡潔、穩定、好提交**：陣列版通常比較適合。
- **如果目標是貼近題解中 `LinkedList / deque` 的資料結構觀念**：linked-list 版比較貼切。

### 結論

- **陣列版優點**：簡單、快、常數成本低、比較好寫。
- **linked-list 版優點**：更接近題解的容器概念，也更能表現 queue 的動態結構。
- **實戰上**：這題我會優先選陣列版。

---

## 空間複雜度的差異

如果用一般演算法分析方式來看：

- [933_Number of Recent Calls.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/933_Number of Recent Calls/933_Number of Recent Calls.c)：空間複雜度是 `O(n)`
- [933_Number of Recent Calls_linked-list.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/933_Number of Recent Calls/933_Number of Recent Calls_linked-list.c)：空間複雜度也是 `O(n)`

原因很簡單，兩者本質上都要儲存「目前滑動視窗內還有效的請求」。

所以從大方向來看：

- **理論上的空間複雜度一樣**
- **兩者都不是因為 linked list 就會變成更低的 Big-O**

真正的差別在「實際記憶體開銷」：

- **陣列版**：資料放在連續記憶體中，主要就是儲存每個時間值。
- **linked-list 版**：每個節點除了時間值，還要額外存一個 `next` 指標，所以單一元素的成本更高。

也就是說：

- **Big-O 一樣都是 `O(n)`**
- **但 linked-list 版通常會比陣列版更吃記憶體**

如果把題目限制也一起考慮進來，會有另一種寫法：

- 題目保證 `t` 嚴格遞增
- 視窗只保留最近 3000 毫秒內的請求
- 因此有效元素數量存在上界

所以在這題的特定語境下，有些題解會把空間複雜度寫成 **`O(1)`**，意思是「視窗大小上限可視為常數」。

因此，最完整的說法是：

- **一般演算法分析**：兩者都是 `O(n)`
- **依照本題固定限制來看**：兩者都可以視為 `O(1)`
- **實際記憶體用量比較**：linked-list 版通常大於陣列版
