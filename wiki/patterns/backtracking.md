# Backtracking 回溯法模式

## 核心概念

回溯法是一種**遞迴技術**，系統地探索問題的所有可能解。當發現一條路線無法達成目標時，立即**撤回並嘗試其他路線**。

**何時使用：**
- 尋找**所有可能的解**而非單一最優解
- 問題具有**層級結構**（樹形選擇空間）
- 需要**條件剪枝**來優化搜索

## 核心模式

### 1. 遞迴路徑探索（Recursive Path Exploration）

```c
// 17. Letter Combinations of a Phone Number (Medium)
// 九宮格鍵盤映射，找所有可能的字母組合

#include <stdlib.h>
#include <string.h>

char** letterCombinations(char* digits, int* returnSize) {
    if (!digits || strlen(digits) == 0) {
        *returnSize = 0;
        return NULL;
    }

    // 數字到字母的映射
    const char *map[10] = {
        "",        // 0
        "",        // 1
        "abc",     // 2
        "def",     // 3
        "ghi",     // 4
        "jkl",     // 5
        "mno",     // 6
        "pqrs",    // 7
        "tuv",     // 8
        "wxyz"     // 9
    };

    // 計算結果個數
    int count = 1;
    for (int i = 0; digits[i]; i++) {
        count *= strlen(map[digits[i] - '0']);
    }

    char **result = malloc(count * sizeof(char*));
    for (int i = 0; i < count; i++) {
        result[i] = malloc(strlen(digits) + 1);
    }

    // 回溯函數
    void backtrack(int index, char *path) {
        // 基礎情況：已選完所有數字
        if (index == strlen(digits)) {
            strcpy(result[*returnSize], path);
            (*returnSize)++;
            return;
        }

        // 取當前數字對應的字母
        const char *letters = map[digits[index] - '0'];

        // 探索每個字母
        for (int i = 0; letters[i]; i++) {
            path[index] = letters[i];      // 選擇
            backtrack(index + 1, path);    // 探索
            // 隱式回溯：透過指針覆蓋即可（路徑被重用）
        }
    }

    *returnSize = 0;
    char *path = malloc(strlen(digits) + 1);
    path[strlen(digits)] = '\0';
    backtrack(0, path);
    free(path);

    return result;
}
```

**複雜度：** O(4^n * n) 時間（最多 4^n 個結果，每個複製 O(n)），O(n) 遞迴深度
**關鍵點：**
- 使用**單一共享路徑**（而不是為每層創建新路徑）
- 基礎情況：已選完所有數字

---

## 回溯的通用模板

```c
// 通用回溯模板（偽代碼轉換為 C）
void backtrack(int index, char *path, int *pathLen, char **result, int *resultSize) {
    // 1. 基礎情況：完成當前組合
    if (index == inputSize) {
        // 記錄答案
        char *copy = malloc(*pathLen + 1);
        strncpy(copy, path, *pathLen);
        copy[*pathLen] = '\0';
        result[(*resultSize)++] = copy;
        return;
    }

    // 2. 選擇階段：嘗試所有可能的下一步選擇
    for (int choice in getChoices(index)) {
        // 2a. 進行選擇
        path[*pathLen] = choice;
        (*pathLen)++;

        // 2b. 遞迴探索
        backtrack(index + 1, path, pathLen, result, resultSize);

        // 2c. 撤銷選擇（顯式回溯）
        (*pathLen)--;
    }
}
```

---

## 回溯 vs. 遞迴的區別

| 特性 | 遞迴 | 回溯 |
|------|------|------|
| 目的 | 分治/縮小問題 | 探索所有解 |
| 決策樹 | 線性或分支 | 完全探索所有分支 |
| 回歸 | 無須撤銷 | 必須撤銷選擇 |
| 結果數 | 1 個或 0 個 | 多個 |

---

## 常見回溯問題分類

### 組合型回溯（Combination）
- **目標：** 從 n 個元素選 k 個
- **剪枝：** 檢查是否有足夠元素剩餘

### 排列型回溯（Permutation）
- **目標：** 排列 n 個元素
- **剪枝：** 追蹤已用元素，跳過重複

### 分割型回溯（Partition）
- **目標：** 將序列分割成多個部分
- **剪枝：** 檢查當前分割是否有效

### 路徑型回溯（Path Finding）
- **目標：** 在樹/圖中找所有路徑
- **剪枝：** 避免環、訪問已節點

---

## 優化技巧

### 1. 剪枝（Pruning）

```c
// 提前終止無望的分支
if (currentSum > targetSum) {
    return;  // 剪枝：已超出目標，無須繼續
}
```

### 2. 去重（Deduplication）

```c
// 避免探索重複的組合
if (i > 0 && candidates[i] == candidates[i-1] && !used[i-1]) {
    continue;  // 跳過重複元素
}
```

### 3. 預排序

```c
// 排序後可更有效地剪枝
qsort(candidates, candidatesSize, sizeof(int), compare);
```

---

## 常見陷阱

1. **忘記撤銷選擇**：導致後續分支受污染
2. **深度無限制**：需要明確的基礎情況
3. **重複計算**：相同的子問題被重複探索
4. **記憶體洩漏**：動態分配的結果需正確 free

---

## 相關 LeetCode 題目

| # | 題名 | 難度 | 回溯類型 |
|---|------|------|----------|
| 17 | Letter Combinations of a Phone Number | Med | 遞迴路徑探索 |
| 39 | Combination Sum | Med | 組合型回溯 |
| 46 | Permutations | Med | 排列型回溯 |
| 51 | N-Queens | Hard | 路徑型回溯+剪枝 |
| 77 | Combinations | Med | 組合型回溯 |

---

## 面試要點

1. **清楚解釋選擇、探索、撤銷的三個步驟**
2. **討論時間複雜度**：通常是 O(k * N^k) 或更高
3. **剪枝策略**：為何能安全去掉某些分支
4. **與 DP 對比**：什麼時候用回溯，什麼時候用 DP
5. **記憶體管理**：特別是在 C 中，如何避免洩漏

## 實現檢查清單

- [ ] 基礎情況明確（何時停止遞迴）
- [ ] 選擇階段完整（列舉所有可能）
- [ ] 撤銷選擇正確（與選擇操作對稱）
- [ ] 剪枝邏輯合理（不漏掉有效解）
- [ ] 記憶體正確分配和釋放
- [ ] 測試邊界情況（空輸入、單一元素）
