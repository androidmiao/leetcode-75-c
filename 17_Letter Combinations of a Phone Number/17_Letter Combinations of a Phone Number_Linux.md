# LeetCode 17: Letter Combinations of a Phone Number — Linux 核心連結分析

## 1. Linux 相關原始碼位置

本題的回溯演算法在 Linux 核心中有多個類似的應用場景。以下列舉核心與該題最為接近的原始碼位置及其相似概念：

### 1.1 `fs/namei.c` — 路徑名解析與符號連結追蹤
**相似性：遞迴式路徑遍歷與狀態回溯**

路徑解析過程中，核心使用類似回溯的邏輯處理符號連結（symlink）：
- 當遇到符號連結時，核心會「保存目前狀態」，轉而追蹤新路徑
- 若新路徑失敗或無效，回溯到上一級狀態重試其他選項
- 最大遞迴深度由 `MAXSYMLINKS` 限制（防止無限循環）

LeetCode #17 中的 `backtrack()` 同樣使用遞迴追蹤，在每層選擇一個字母，狀態由 `index` 與 `path` 共同維持。

**核心程式碼參考：**
```c
// fs/namei.c 中的路徑遍歷邏輯（簡化示意）
while (depth < MAXSYMLINKS) {
    // 嘗試當前路徑分量
    // 若是符號連結，遞迴追蹤
    // 狀態由 nameidata 結構保存
}
```

### 1.2 `fs/readdir.c` 與 VFS 目錄迭代 — 列舉與組合展開
**相似性：遍歷所有可能的選項集合**

目錄迭代器列舉所有目錄項目，過程中：
- 使用迭代器模式一個接一個地返回目錄項
- 對應每個目錄項，上層可能進行進一步的遞迴操作（如 `walk_tree`）
- 狀態保存在 `struct dir_context` 及迭代器內部

本題中的 `for (int i = 0; i < lettersCount; i++)` 迴圈類似於目錄遍歷，每次選擇一個字母後遞迴深入。

**核心概念：**
```c
// fs/readdir.c 中的目錄遍歷模式
for (each entry in directory) {
    ctx->pos = next_offset;  // 維持狀態位置
    // 應用操作於當前項
    // 可能進一步遞迴（如樹形走訪）
}
```

### 1.3 `lib/glob.c` — Shell 型式模式展開（Glob）
**相似性：組合式地展開萬用字元為所有匹配路徑**

Shell glob（如 `ls *.txt`）使用類似的演算法：
- 對每個路徑分量中的模式字元進行展開
- 產生所有符合的組合
- 類似 `"[2-3][0-9]"` 等模式的展開方式

該題的「數字→字母映射→所有組合」與 glob 的「通配符→所有匹配路徑」邏輯等同。

**核心概念：**
```
Pattern: "23" (with 2→abc, 3→def)
Expansion: [a|b|c][d|e|f] → {ad, ae, af, bd, be, bf, cd, ce, cf}
```

### 1.4 `drivers/input/input.c` — 輸入裝置事件對應
**相似性：輸入代碼到字元的映射**

輸入子系統將物理按鍵按下事件對應到符號：
- 按鍵掃描碼（scancode）經過數個映射層最終變成 Unicode 字元
- 支援國際化鍵盤時，同一按鍵在不同鍵盤配置下對應不同字元
- 映射表通常使用陣列或雜湊表存儲

本題的 `PHONE_MAP[]` 直接對應此概念：固定的按鍵→字母映射表。

### 1.5 `include/linux/input-event-codes.h` — 輸入事件代碼定義
**相似性：預先定義的對應表**

該標頭檔定義所有可能的輸入事件代碼與其對應意義，類似於本題預先定義的 `PHONE_MAP`。

---

## 2. 核心 vs. LeetCode 實作對照

| 維度 | Linux 核心範例 | LeetCode #17 解法 |
|------|---|---|
| **演算法模式** | 遞迴式路徑追蹤（namei.c）/ 迭代列舉（readdir.c） | 遞迴式回溯搜尋 |
| **狀態維持** | `struct nameidata` / `dir_context` | `index` + `path` 字串 |
| **映射策略** | 陣列索引（按鍵→事件）/ 雜湊表（路徑→inode） | 靜態常量陣列 `PHONE_MAP[]` |
| **邊界條件** | `MAXSYMLINKS` 防止無限遞迴 | `index == digitsSize` 作為終止條件 |
| **遞迴 vs. 反覆** | 核心傾向使用反覆式（避免堆疊溢位） | 使用遞迴（清晰易懂，深度可控） |
| **記憶體配置** | 核心使用 `kmalloc()`/ SLAB 分配器 | 使用 `malloc()` 配置答案陣列 |
| **錯誤處理** | 檢查返回值，完整的錯誤路徑 | 簡化，主要檢查空字串輸入 |

### 2.1 演算法模式對照

**核心做法（fs/namei.c 路徑解析）：**
```
解析路徑 "/a/b/c"
├─ 查詢分量 "a"
│  └─ 若是符號連結，保存狀態，追蹤新路徑
│     ├─ 若成功，繼續
│     └─ 若失敗，回溯並嘗試下一選項
├─ 查詢分量 "b"
│  └─ 同上
└─ 查詢分量 "c"
```

**LeetCode #17 做法（回溯組合）：**
```
組合 "23"
├─ 選擇來自 "2" 的字母 (a/b/c)
│  └─ 針對每個選擇，進入下一層
│     ├─ 選擇來自 "3" 的字母 (d/e/f)
│     │  └─ 產生組合 (ad/ae/af/...)
│     └─ 回溯，改選下一字母
└─ 完成
```

### 2.2 狀態維持對照

**核心方式：**
- 使用 `struct nameidata` 保存當前解析狀態：目前目錄 inode、已解析路徑、剩餘路徑等
- 多個指標和物理地址，支援複雜狀態轉移

**LeetCode 做法：**
- 使用簡化的狀態表示：
  - `index`：目前正在處理的數字位置
  - `path`：目前已組合的字母字串
  - 遞迴堆疊隱含保存中間狀態

---

## 3. 主管 Code Review 角度考題

一位熟悉 Linux 核心的資深工程師進行程式碼審查時，可能會提出以下問題及其評估意圖：

### 問題 1：遞迴深度與堆疊溢位風險
**提問內容：**
```
你注意到回溯函式的遞迴深度上限嗎？在核心程式碼中，我們用 MAXSYMLINKS
來限制符號連結深度。此處沒有設定遞迴深度上限，萬一 digits 長度異常大會發生什麼？
```

**預期答案：**
- LeetCode 問題保證 `0 <= digits.length <= 4`，因此遞迴深度最多 4 層，不構成實務風險
- 若應用於生產環境，應在函式入口處檢查 `digitsSize > MAX_DEPTH` 並返回錯誤

**進階追問：**
- 「如何用反覆式（stack 模擬遞迴）來改寫此函式？」
  - 評估：了解反覆式改寫與堆疊管理
- 「核心中 fs/namei.c 為何傾向反覆式而非遞迴式？」
  - 評估：理解核心優化考量（避免堆疊溢位、效能）

### 問題 2：靜態常量陣列 vs. 動態雜湊表
**提問內容：**
```
你為什麼選擇使用靜態常量陣列 PHONE_MAP[]？如果支援國際化鍵盤配置，
需要在執行時動態載入映射表，應如何重構？
```

**預期答案：**
- 靜態陣列的優點：
  - 編譯時確定，無執行時配置開銷
  - 快速查詢（O(1) 索引）
  - 記憶體使用預測性好
- 國際化需求場景：
  ```c
  // 簡化版：動態映射表
  struct PhoneMap {
      int digit;
      const char *letters;
  } *dynamic_map;  // 執行時載入
  ```

**進階追問：**
- 「核心的輸入子系統（drivers/input/）中，鍵盤配置如何動態變更？」
  - 評估：認識事件驅動架構與延遲繫結
- 「若映射表特別大，應如何最佳化記憶體存取？」
  - 評估：快取感知性（cache locality）、記憶體佈局

### 問題 3：字串緩衝區管理與越界風險
**提問內容：**
```
path 的大小設定為 digitsSize + 1。你如何確保 strlen(PHONE_MAP[digit])
的結果永遠不會超出預期？在核心中，此類字串操作如何防護？
```

**預期答案：**
- 靜態檢查：PHONE_MAP 中最長字母序列為 "pqrs"（4 字元）
- digitsSize 最大 4，path 最大長度為 4+1=5，緩衝區足夠
- 若輸入來自使用者，應驗證 `digitsSize <= MAX_DIGITS`

**進階追問：**
- 「strcpy() 被認為不安全，為何此處可接受？」
  - 評估：理解緩衝區大小已事先計算的邏輯
  - 更安全做法：`strncpy()` 或 `snprintf()`
- 「核心中如何使用 `strlcpy()` 替代？」
  - 評估：認識 BSD 安全函式庫

### 問題 4：記憶體配置與釋放策略
**提問內容：**
```
函式返回 result 指標，但呼叫者需自行負責逐個 free 各字串及 result 本身。
核心中如何處理類似的「分散式」記憶體管理？
```

**預期答案：**
- LeetCode 環境下，測試框架統一釋放
- 生產環境應返回結構體（如 `Result` 包含陣列指標和大小），清楚標示擁有權
- 使用 RAII 或物件管理慣例

**進階追問：**
- 「若改為單一記憶體塊存儲所有字串（如 pool allocator），如何實現？」
  - 評估：記憶體佈局最佳化、快速釋放
- 「核心使用何種機制追蹤動態配置的生命週期？」
  - 評估：認識 kmemleak、refcount 等工具

### 問題 5：計算組合數時的溢位風險
**提問內容：**
```
totalCombinations 計算中，totalCombinations *= strlen(PHONE_MAP[digit])
可能溢位。如何防護？
```

**預期答案：**
- LeetCode 保證結果不超過 `2^31 - 1`（int 範圍）
- 生產環境應檢查：
  ```c
  if (totalCombinations > INT_MAX / lettersCount) {
      // 溢位風險
      return error;
  }
  totalCombinations *= lettersCount;
  ```

**進階追問：**
- 「核心中 size_t 的用途是什麼？」
  - 評估：無符號整數與溢位檢查
- 「使用 __builtin_mul_overflow() 如何改進？」
  - 評估：認識編譯器內建函式

### 問題 6：時間與空間複雜度分析
**提問內容：**
```
此演算法的時間與空間複雜度是多少？若 digits 長度為 n，最壞情況下複雜度如何？
```

**預期答案：**
- **時間複雜度：** O(4^n · n)
  - 生成 4^n 個組合（每位最多 4 個字母，如 7 或 9）
  - 複製每個組合時 O(n)
- **空間複雜度：** O(4^n)
  - 答案陣列存儲所有組合
  - 遞迴堆疊深度 O(n)

**進階追問：**
- 「核心中路徑名解析的複雜度分析？」
  - 評估：理解路徑深度、符號連結追蹤的影響
- 「如何使用動態規劃來最佳化此問題？」
  - 評估：更深層的演算法理解（本題不適用，但能展現思考廣度）

### 問題 7：邊界情況與錯誤處理
**提問內容：**
```
你的實作檢查了空字串和 NULL 輸入。還有其他邊界情況嗎？
例如，digits 包含 '0' 或 '1' 這樣沒有對應字母的數字時？
```

**預期答案：**
- 當遇到 '0' 或 '1'，PHONE_MAP[0] 和 PHONE_MAP[1] 皆為空字串
- `strlen("")` 返回 0，`for` 迴圈不執行
- 結果：該分支不產生任何組合，最終返回空陣列
- 預期行為：正確（與題意一致）

**進階追問：**
- 「如何編寫單元測試覆蓋所有邊界情況？」
  - 評估：軟體工程實踐
- 「核心在系統呼叫入口如何驗證使用者輸入？」
  - 評估：安全邊界認識

---

## 4. 面試加分總結

### 4.1 與核心知識的連結
1. **算法模式對應**
   - 能識別 LeetCode 回溯解法與 Linux 路徑解析、目錄遍歷的相似之處
   - 展現跨領域的系統化思維

2. **深度優化考量**
   - 討論反覆式 vs. 遞迴式的取捨（核心傾向反覆式以避免堆疊溢位）
   - 展現對系統資源（堆疊、CPU 快取）的理解

3. **記憶體安全**
   - 能辨識可能的緩衝區溢位、整數溢位風險
   - 了解生產級程式碼應如何防護

### 4.2 實務應用案例
- **路徑列舉**：類似 Linux 中 `find` 或 `walk_tree` 的遞迴遍歷
- **組合式展開**：Shell glob、正則運算式、國際化鍵盤配置等
- **狀態機設計**：回溯法本質上是狀態機，核心中廣泛應用

### 4.3 回答策略
在面試中，提及以下重點能獲得加分：

1. **主動涉及核心知識**
   > 「這個回溯演算法與 Linux 中 fs/namei.c 的路徑追蹤邏輯相似。
   > 不過核心使用反覆式實作以防止深度遞迴引起的堆疊溢位。
   > 在 LeetCode 環境下，由於深度上限已知（≤4），遞迴是可接受的。」

2. **討論設計取捨**
   > 「使用靜態陣列映射很快，但如果需要動態鍵盤配置，
   > 就需要改用雜湊表或延遲繫結機制，類似輸入子系統的做法。」

3. **補充防護措施**
   > 「生產環境應檢查：
   >   - digitsSize <= MAX_DEPTH，防止堆疊溢位
   >   - totalCombinations 計算中檢查整數溢位
   >   - 使用 strlcpy() 替代 strcpy()」

4. **展現系統視野**
   > 「這個演算法在 shell glob、檔案系統遍歷、輸入裝置映射等多個領域都有應用。
   > 核心之所以選擇反覆式，是基於堆疊資源的限制；
   > 在應用層則可根據具體約束靈活選擇。」

---

## 5. Sources

### Linux 核心原始碼（torvalds/linux GitHub）

1. **路徑解析與符號連結**
   - https://github.com/torvalds/linux/blob/master/fs/namei.c
   - 關鍵函式：`link_path_walk()`, `walk_component()`, `__follow_mount_rcu()`

2. **目錄迭代**
   - https://github.com/torvalds/linux/blob/master/fs/readdir.c
   - 關鍵結構：`struct dir_context`, `readdir_callback()`

3. **VFS 介面**
   - https://github.com/torvalds/linux/blob/master/include/linux/fs.h
   - 目錄操作、inode 操作等虛擬檔案系統介面

4. **輸入子系統**
   - https://github.com/torvalds/linux/blob/master/drivers/input/input.c
   - 關鍵函式：事件分發、按鍵對應映射

5. **輸入事件代碼**
   - https://github.com/torvalds/linux/blob/master/include/linux/input-event-codes.h
   - KEY_* 定義、事件代碼表

6. **記憶體管理**
   - https://github.com/torvalds/linux/blob/master/mm/slab.c
   - SLAB 配置器、`kmalloc()` 實作

7. **字串操作（安全函式）**
   - https://github.com/torvalds/linux/blob/master/lib/string.c
   - `strlcpy()`, `strlcat()`, 相較 `strcpy()` 的改進

### 補充資源

- **LeetCode 官方討論**：https://leetcode.com/problems/letter-combinations-of-a-phone-number/
- **Linux 核心文件**：https://www.kernel.org/doc/html/latest/
- **「Linux 核心設計與實作」**等專業書籍：深入理解核心中的數據結構與演算法

---

**注記：**
本文旨在展現 LeetCode 演算法題與 Linux 核心設計思想的連結，
幫助面試者在技術深度與系統視野上獲得優勢。
實務應用中，應根據具體場景選擇適當的演算法與資料結構。
