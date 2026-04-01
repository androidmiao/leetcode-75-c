# 1431. Kids With the Greatest Number of Candies — Implementation vs Editorial Comparison

## English Version

### 1. Implementation vs Editorial Comparison

#### Algorithm Overview

Both solutions employ the **two-pass linear scan** algorithm:
1. First pass: find the maximum number of candies (`greatest` or `maxCandies`)
2. Second pass: for each kid, determine whether they would have the maximum after receiving extra candies

| Aspect | Repository Solution | LeetCode Editorial |
|--------|---------------------|-------------------|
| **Language** | C (C99) | Language-agnostic (Ad Hoc) |
| **Initialization** | `greatest = -1` | `maxCandies = 0` |
| **Data Structure** | `bool* table` (malloc'd) | Result array (language-dependent) |
| **Null Check** | Yes, on `candies` pointer | Implicit in editorial |
| **Boolean Assignment** | Explicit if-else block | Direct comparison expression |
| **sizeof Bug** | Yes: `sizeof(table)` instead of `sizeof(*table)` | N/A |
| **Time Complexity** | O(n) | O(n) |
| **Space Complexity** | O(n) for output | O(n) for output |
| **Return Mechanism** | Returns `bool*` array | Pushes results into output array |

#### Code Structure Comparison

**Repository Solution (solution.c):**
```c
bool* kidsWithCandies(int* candies, int candiesSize, int extraCandies,
                      int* returnSize) {
    bool* table = (bool*)malloc(sizeof(table)); // BUG: sizeof(table) is sizeof(bool*)
    if (!candies) return NULL;

    *returnSize = candiesSize;

    // First pass: find greatest
    int greatest = -1;
    for (int i = 0; i < candiesSize; i++) {
        if (candies[i] > greatest) {
            greatest = candies[i];
        }
    }

    // Second pass: assign boolean results
    for (int i = 0; i < candiesSize; i++) {
        if (candies[i] + extraCandies >= greatest) {
            table[i] = true;
        } else {
            table[i] = false;
        }
    }

    return table;
}
```

**LeetCode Editorial Approach:**
```
1. Initialize maxCandies = 0
2. Iterate through candies array, update maxCandies = max(maxCandies, candy)
3. Iterate again, push (candy + extraCandies >= maxCandies) into result
4. Return result array
```

#### Key Differences

1. **Initialization Value**
   - Repository: `greatest = -1` (defensive, works because constraints guarantee `candies[i] ≥ 1`)
   - Editorial: `maxCandies = 0` (simpler, relies on constraint `candies[i] ≥ 1`)

2. **Boolean Assignment**
   - Repository: Explicit if-else (clear intent, slightly more verbose)
   - Editorial: Direct comparison expression (more concise, idiomatic)

3. **sizeof Bug (Critical Code Review Finding)**
   - **Repository Code:** `sizeof(table)` allocates `sizeof(bool*)` bytes
     - On 64-bit systems: typically 8 bytes
     - Should allocate: `candiesSize * sizeof(bool)` = `candiesSize * 1` byte
     - Example: For `candiesSize = 5`, allocates 8 bytes instead of 5 bytes
     - **Result:** Over-allocation by 37.5% (wasteful but doesn't crash)
   - **Correct Version:** `malloc(candiesSize * sizeof(bool))` or `malloc(candiesSize * sizeof(*table))`

### 2. Which Approach is Better?

#### Time Complexity Analysis
- **Both:** O(n) — exactly two full passes through the array
- **Verdict:** Tied

#### Space Complexity Analysis
- **Both:** O(n) for the output array (required by problem)
- **Additional space:** Both use O(1) auxiliary variables
- **Verdict:** Tied

#### Interview Suitability
| Criterion | Repository | Editorial |
|-----------|------------|-----------|
| **Correctness** | Functionally correct despite sizeof bug | Correct |
| **Code clarity** | Good structure, explicit if-else | More idiomatic |
| **Error handling** | Checks null candies | Assumes valid input |
| **Production readiness** | sizeof bug makes it problematic | Ready as-is |
| **Demonstration value** | Shows defensive programming | Shows clean, direct logic |

#### C Implementation Difficulty
- **Repository:** Medium (malloc/free management, sizeof considerations)
- **Editorial:** Medium (same malloc/free management)
- **Key Challenge:** Both require careful management of dynamic memory allocation and understanding of `sizeof` operator

### 3. Baseline vs Recommended Approaches

**Baseline Approach: Two-Pass with Defensive Initialization**
- Initialize `greatest = -1`
- Use explicit if-else for clarity
- Demonstrates safe bounds-checking mindset
- **Status:** Functionally correct but contains sizeof bug

**Recommended Approach: Two-Pass with Idiomatic C**
- Initialize `maxCandies = 0` (relying on constraint)
- Use direct comparison assignment: `table[i] = (candies[i] + extraCandies >= maxCandies);`
- Allocate with: `malloc(candiesSize * sizeof(*table))` or `malloc(candiesSize * sizeof(bool))`
- Add comprehensive error handling
- **Advantages:**
  - Cleaner, more idiomatic C code
  - Fixes the sizeof bug
  - Reduces memory waste
  - Maintains same time/space complexity

#### Code Review Summary
| Issue | Severity | Impact |
|-------|----------|--------|
| `sizeof(table)` bug | High | Over-allocation by ~37.5% on typical systems |
| Null check | Low | Good defensive practice, though candies guaranteed non-null by API |
| if-else verbosity | Low | Minor style issue; not incorrect |
| Overall correctness | Medium | Works despite bug, but should be fixed |

---

# 繁體中文版本

## 1431. Kids With the Greatest Number of Candies — 實現方案與官方社論比較

### 1. 實現方案與官方社論比較

#### 演算法概述

兩個解決方案都採用**兩遍線性掃描**演算法：
1. 第一遍：找到最大糖果數量（`greatest` 或 `maxCandies`）
2. 第二遍：對於每個孩子，判斷接收額外糖果後是否擁有最大值

| 面向 | 存儲庫解決方案 | LeetCode 官方社論 |
|------|-------------|----------------|
| **編程語言** | C (C99) | 語言無關（Ad Hoc） |
| **初始化值** | `greatest = -1` | `maxCandies = 0` |
| **數據結構** | `bool* table`（malloc 分配） | 結果陣列（語言相關） |
| **空指針檢查** | 是的，檢查 `candies` | 官方社論隱含 |
| **布爾賦值** | 顯式 if-else 塊 | 直接比較運算式 |
| **sizeof 漏洞** | 是的：`sizeof(table)` 而非 `sizeof(*table)` | 不存在 |
| **時間複雜度** | O(n) | O(n) |
| **空間複雜度** | O(n) 用於輸出 | O(n) 用於輸出 |
| **返回機制** | 返回 `bool*` 陣列 | 將結果推入輸出陣列 |

#### 代碼結構比較

**存儲庫解決方案（solution.c）：**
```c
bool* kidsWithCandies(int* candies, int candiesSize, int extraCandies,
                      int* returnSize) {
    bool* table = (bool*)malloc(sizeof(table)); // 漏洞：sizeof(table) 是 sizeof(bool*)
    if (!candies) return NULL;

    *returnSize = candiesSize;

    // 第一遍：找到最大值
    int greatest = -1;
    for (int i = 0; i < candiesSize; i++) {
        if (candies[i] > greatest) {
            greatest = candies[i];
        }
    }

    // 第二遍：分配布爾結果
    for (int i = 0; i < candiesSize; i++) {
        if (candies[i] + extraCandies >= greatest) {
            table[i] = true;
        } else {
            table[i] = false;
        }
    }

    return table;
}
```

**LeetCode 官方社論方法：**
```
1. 初始化 maxCandies = 0
2. 遍歷糖果陣列，更新 maxCandies = max(maxCandies, candy)
3. 再次迭代，將 (candy + extraCandies >= maxCandies) 推入結果
4. 返回結果陣列
```

#### 主要差異

1. **初始化值**
   - 存儲庫：`greatest = -1`（防御性，因為約束條件保證 `candies[i] ≥ 1`）
   - 官方社論：`maxCandies = 0`（更簡潔，依賴約束條件 `candies[i] ≥ 1`）

2. **布爾賦值**
   - 存儲庫：顯式 if-else（清晰意圖，略顯冗長）
   - 官方社論：直接比較運算式（更簡潔，更習慣用語）

3. **sizeof 漏洞（關鍵代碼審查發現）**
   - **存儲庫代碼：** `sizeof(table)` 分配 `sizeof(bool*)` 位元組
     - 在 64 位系統上：通常為 8 位元組
     - 應分配：`candiesSize * sizeof(bool)` = `candiesSize * 1` 位元組
     - 例如：對於 `candiesSize = 5`，分配 8 位元組而非 5 位元組
     - **結果：** 多分配 37.5%（浪費但不會崩潰）
   - **正確版本：** `malloc(candiesSize * sizeof(bool))` 或 `malloc(candiesSize * sizeof(*table))`

### 2. 哪種方法更好？

#### 時間複雜度分析
- **兩者均為：** O(n) — 恰好兩次完整遍歷陣列
- **結論：** 並列

#### 空間複雜度分析
- **兩者均為：** O(n) 用於輸出陣列（問題要求）
- **額外空間：** 兩者都使用 O(1) 輔助變數
- **結論：** 並列

#### 面試適用性
| 標準 | 存儲庫 | 官方社論 |
|------|------|--------|
| **正確性** | 儘管有 sizeof 漏洞，功能上仍正確 | 正確 |
| **代碼清晰度** | 結構良好，顯式 if-else | 更符合習慣用語 |
| **錯誤處理** | 檢查 null candies | 假設有效輸入 |
| **生產就緒性** | sizeof 漏洞使其有問題 | 準備就緒 |
| **演示價值** | 展示防御性編程 | 展示簡潔直接的邏輯 |

#### C 語言實現難度
- **存儲庫：** 中等（malloc/free 管理，sizeof 注意事項）
- **官方社論：** 中等（同樣的 malloc/free 管理）
- **主要挑戰：** 兩者都要求仔細管理動態記憶體分配並理解 `sizeof` 運算符

### 3. 基礎方案與推薦方案

**基礎方案：帶防御性初始化的兩遍掃描**
- 初始化 `greatest = -1`
- 使用顯式 if-else 以求清晰
- 展示安全邊界檢查的思維
- **狀態：** 功能上正確但包含 sizeof 漏洞

**推薦方案：符合習慣用語的兩遍掃描**
- 初始化 `maxCandies = 0`（依賴約束條件）
- 使用直接比較賦值：`table[i] = (candies[i] + extraCandies >= maxCandies);`
- 使用以下方式分配：`malloc(candiesSize * sizeof(*table))` 或 `malloc(candiesSize * sizeof(bool))`
- 添加全面的錯誤處理
- **優勢：**
  - 更簡潔，更符合習慣用語的 C 代碼
  - 修復 sizeof 漏洞
  - 減少記憶體浪費
  - 保持相同的時間/空間複雜度

#### 代碼審查摘要
| 問題 | 嚴重程度 | 影響 |
|------|--------|------|
| `sizeof(table)` 漏洞 | 高 | 在典型系統上多分配約 37.5% |
| 空指針檢查 | 低 | 良好的防御實踐，儘管 API 保證 candies 非空 |
| if-else 冗長 | 低 | 輕微的風格問題；並非不正確 |
| 整體正確性 | 中等 | 儘管有漏洞仍可運作，但應修復 |
