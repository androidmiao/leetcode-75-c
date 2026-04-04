# Binary Search 二分搜尋模式

## 概述 Overview

二分搜尋是解決排序陣列中查找問題的核心演算法。通過每次將搜尋範圍縮小一半，將時間複雜度從線性的 O(n) 降低到 O(log n)。該模式涵蓋標準二分搜尋、旋轉陣列搜尋、峰值搜尋等多種變體。

二分搜尋的關鍵是正確處理邊界條件和避免溢位問題。特別在 C 語言中，mid 的計算必須使用防溢位公式 `mid = left + (right - left) / 2`。

## 核心概念 Core Concepts

### 1. 標準二分搜尋模板

有兩種主要模板：左閉右開 [left, right) 和雙閉 [left, right]。

**左閉右開模板（推薦）：**
```c
int binary_search(int* nums, int numsSize, int target) {
    int left = 0, right = numsSize;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] < target) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left < numsSize && nums[left] == target ? left : -1;
}
```

**雙閉模板：**
```c
int binary_search_closed(int* nums, int numsSize, int target) {
    int left = 0, right = numsSize - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] == target) {
            return mid;
        } else if (nums[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1;
}
```

### 2. 防溢位計算

```c
// 錯誤的方式
int mid = (left + right) / 2;  // 可能導致整數溢位

// 正確的方式
int mid = left + (right - left) / 2;
// 或
int mid = (unsigned)(left + right) >> 1;
```

### 3. 邊界條件處理

在變體問題中，選擇正確的邊界條件至關重要：
- 查找確切值：檢查 nums[mid] == target
- 查找左邊界：檢查 nums[mid] >= target，移動 right = mid
- 查找右邊界：檢查 nums[mid] <= target，移動 left = mid + 1
- 查找峰值：檢查 nums[mid] > nums[mid+1]

## 常見變體 Common Variants

### 變體 1：旋轉排序陣列搜尋 (Problem 33)

旋轉陣列有兩個有序段。關鍵是確定哪個段是有序的，然後決定目標在哪裡。

```c
int search(int* nums, int numsSize, int target) {
    int left = 0, right = numsSize - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (nums[mid] == target) {
            return mid;
        }

        // 判斷左段是否有序
        if (nums[left] <= nums[mid]) {
            // 左段有序，目標在左段
            if (target >= nums[left] && target < nums[mid]) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        } else {
            // 右段有序，目標在右段
            if (target > nums[mid] && target <= nums[right]) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
    }
    return -1;
}
```

### 變體 2：搜尋插入位置 (Problem 35)

找到排序陣列中目標值的位置，如果不存在則返回應該插入的位置。

```c
int searchInsert(int* nums, int numsSize, int target) {
    int left = 0, right = numsSize;

    while (left < right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] < target) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left;  // 直接返回 left，既是存在的索引，也是插入位置
}
```

### 變體 3：峰值元素搜尋 (Problem 162)

在陣列中找到峰值（比兩個鄰居都大的元素）。陣列兩端可視為 -∞。

```c
int findPeakElement(int* nums, int numsSize) {
    int left = 0, right = numsSize - 1;

    while (left < right) {
        int mid = left + (right - left) / 2;

        if (nums[mid] > nums[mid + 1]) {
            // 峰值在左側或就是 mid
            right = mid;
        } else {
            // 峰值在右側
            left = mid + 1;
        }
    }
    return left;
}
```

### 變體 4：在答案上進行二分搜尋 (Problem 374)

不是在陣列中搜尋，而是在可能的答案上進行二分搜尋。

```c
int guessNumber(int n) {
    int left = 1, right = n;

    while (left < right) {
        int mid = left + (right - left) / 2;
        int result = guess(mid);

        if (result == 0) {
            return mid;
        } else if (result > 0) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left;
}
```

## 相關問題 Related Problems

| 問題 ID | 名稱 | 難度 | 描述 |
|---------|------|------|------|
| 33 | Search in Rotated Sorted Array | Med | 在旋轉的排序陣列中搜尋 |
| 35 | Search Insert Position | Easy | 搜尋插入位置 |
| 162 | Find Peak Element | Med | 使用分治搜尋峰值 |
| 374 | Guess Number Higher or Lower | Easy | 標準二分搜尋猜數遊戲 |
| 704 | Binary Search | Easy | 最基本的二分搜尋 |

## 內核連接 Kernel Connection

Linux 內核中使用的二分搜尋實現位於 `lib/bsearch.c`。該實現使用 `(base + size / 2)` 的方式計算中點位置，避免整數溢位。內核中的很多子系統都依賴於高效的搜尋算法，如進程管理、記憶體管理和檔案系統。

## 時間和空間複雜度

- **時間複雜度**：O(log n)
  - 標準二分搜尋：每次縮小範圍一半
  - 旋轉陣列搜尋：同樣是 O(log n)
  - 峰值搜尋：O(log n)

- **空間複雜度**：O(1)
  - 只使用常數額外空間

## 除錯技巧 Debugging Tips

1. **驗證邊界**：測試陣列的開始、結束和中間元素
2. **測試邊界情況**：
   - 單個元素的陣列
   - 目標不在陣列中
   - 目標是最小或最大值
3. **檢查溢位**：使用防溢位的 mid 計算公式
4. **驗證迴圈條件**：確保迴圈最終會終止
5. **列印除錯**：在關鍵步驟列印 left、right 和 mid 的值

## 實踐建議 Practice Tips

1. 從最簡單的標準二分搜尋開始
2. 熟悉兩種模板（左閉右開和雙閉），選擇一種並深入掌握
3. 對於變體問題，先確定是否可以應用二分搜尋
4. 在變體中，重點關注邊界條件如何改變
5. 練習在各種數據上手工執行二分搜尋
6. 理解為什麼某些條件下某一邊是安全的

## 常見錯誤 Common Mistakes

1. **整數溢位**：使用 (left + right) / 2 而不是 left + (right - left) / 2
2. **邊界錯誤**：混淆 left 和 right 的更新方式
3. **條件判斷錯誤**：使用錯誤的比較操作符
4. **無限迴圈**：邊界更新邏輯錯誤導致 left 和 right 無法收斂
5. **返回值錯誤**：忘記檢查是否找到了目標值
