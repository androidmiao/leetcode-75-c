# String Manipulation 字串操作模式

## 核心概念

字串操作包括原地變換、模式匹配、編碼轉換等。在 C 中，效率和記憶體管理都是關鍵考量。

**何時使用：**
- 反轉、旋轉字串
- 尋找模式或特定結構
- 合併、分割、去重字串

## 核心模式

### 1. 原地字元反轉（In-Place Character Reversal）

```c
// 151. Reverse Words in a String (Medium)
// 反轉句子中單詞的順序

#include <string.h>
#include <ctype.h>

char* reverseWords(char* s) {
    int sLen = strlen(s);

    // 移除多餘空格，構建新字串
    char *cleaned = malloc(sLen + 1);
    int cleanIdx = 0;
    int inSpace = 1;  // 追蹤是否在空格中

    for (int i = 0; i < sLen; i++) {
        if (isspace(s[i])) {
            if (!inSpace && cleanIdx > 0) {
                cleaned[cleanIdx++] = ' ';  // 單個空格分隔
                inSpace = 1;
            }
        } else {
            cleaned[cleanIdx++] = s[i];
            inSpace = 0;
        }
    }

    // 移除尾部空格
    if (cleanIdx > 0 && isspace(cleaned[cleanIdx - 1])) {
        cleanIdx--;
    }
    cleaned[cleanIdx] = '\0';

    // 第一步：反轉整個字串
    for (int i = 0, j = cleanIdx - 1; i < j; i++, j--) {
        char temp = cleaned[i];
        cleaned[i] = cleaned[j];
        cleaned[j] = temp;
    }

    // 第二步：反轉每個單詞
    int wordStart = 0;
    for (int i = 0; i <= cleanIdx; i++) {
        if (i == cleanIdx || isspace(cleaned[i])) {
            // 反轉 [wordStart, i-1]
            for (int j = wordStart, k = i - 1; j < k; j++, k--) {
                char temp = cleaned[j];
                cleaned[j] = cleaned[k];
                cleaned[k] = temp;
            }
            wordStart = i + 1;
        }
    }

    return cleaned;
}
```

**複雜度：** O(n) 時間，O(n) 空間
**技巧：**
1. 先反轉整個字串
2. 再反轉每個單詞
3. 結果就是反轉單詞順序

---

### 2. GCD 除數前綴驗證（GCD Divisor Prefix Validation）

```c
// 1071. Greatest Common Divisor of Strings (Easy)
// 找最長的字串使其能重複覆蓋兩個字串

char* gcdOfStrings(char* str1, char* str2) {
    int len1 = strlen(str1);
    int len2 = strlen(str2);

    // 確保 str1 是較長的字串
    if (len1 < len2) {
        return gcdOfStrings(str2, str1);
    }

    // 找兩個長度的最大公因數
    int gcdLen = len2;
    while (len1 % gcdLen != 0 || len2 % gcdLen != 0) {
        gcdLen--;
    }

    // 驗證前 gcdLen 個字元能否重複覆蓋 str1
    char *candidate = malloc(gcdLen + 1);
    strncpy(candidate, str1, gcdLen);
    candidate[gcdLen] = '\0';

    // 檢查 candidate 是否為公共除數
    int valid = 1;

    // 驗證 str1
    for (int i = 0; i < len1; i++) {
        if (str1[i] != candidate[i % gcdLen]) {
            valid = 0;
            break;
        }
    }

    // 驗證 str2
    if (valid) {
        for (int i = 0; i < len2; i++) {
            if (str2[i] != candidate[i % gcdLen]) {
                valid = 0;
                break;
            }
        }
    }

    if (!valid) {
        candidate[0] = '\0';  // 返回空字串
    }

    return candidate;
}
```

**複雜度：** O(m + n) 時間，O(min(m,n)) 空間
**洞察：** 若 GCD 字串存在，其長度必為 gcd(len1, len2)

---

### 3. 雙指標交替合併（Two-Pointer Alternating Merge）

```c
// 1768. Merge Strings Alternately (Easy)
// 交替合併兩個字串的字元

char* mergeAlternately(char* word1, char* word2) {
    int len1 = strlen(word1);
    int len2 = strlen(word2);
    int totalLen = len1 + len2;

    char *result = malloc(totalLen + 1);
    int idx = 0;
    int i = 0, j = 0;

    // 交替添加，直到其中一個字串用完
    while (i < len1 && j < len2) {
        result[idx++] = word1[i++];
        result[idx++] = word2[j++];
    }

    // 添加剩餘字元
    while (i < len1) {
        result[idx++] = word1[i++];
    }
    while (j < len2) {
        result[idx++] = word2[j++];
    }

    result[idx] = '\0';
    return result;
}
```

**複雜度：** O(n + m) 時間，O(n + m) 空間
**技巧：** 一次遍歷即可完成交替合併

---

## 字串操作技巧

### 原地修改（In-Place Modification）

```c
// 適用場景：當允許修改輸入時
void reverseString(char* s, int sSize) {
    for (int i = 0, j = sSize - 1; i < j; i++, j--) {
        char temp = s[i];
        s[i] = s[j];
        s[j] = temp;
    }
}
```

### 避免字串複製

```c
// 使用指標和索引而非複製整個字串
int compareSubstring(char *s, int start, int end, char *pattern) {
    int patIdx = 0;
    for (int i = start; i < end && pattern[patIdx]; i++, patIdx++) {
        if (s[i] != pattern[patIdx]) return 0;
    }
    return pattern[patIdx] == '\0';  // 確認模式完全匹配
}
```

---

## 常用字串函數

| 函數 | 用途 | 時間複雜度 |
|------|------|-----------|
| `strlen` | 長度 | O(n) |
| `strcmp` | 比較 | O(min(m,n)) |
| `strcpy` | 複製 | O(n) |
| `strncpy` | 安全複製 | O(n) |
| `strcat` | 連接 | O(n+m) |
| `strchr` | 尋找字元 | O(n) |
| `strstr` | 尋找子字串 | O(nm) |

---

## C 字串安全注意事項

1. **緩衝區溢出**：總是檢查邊界
   ```c
   char buffer[10];
   strncpy(buffer, source, 9);  // 留一個位置給 '\0'
   buffer[9] = '\0';
   ```

2. **空指標檢查**
   ```c
   if (ptr == NULL) return NULL;
   ```

3. **正確釋放**
   ```c
   char *str = malloc(len + 1);
   // ... 使用 ...
   free(str);
   ```

---

## 相關 LeetCode 題目

| # | 題名 | 難度 | 操作類型 |
|---|------|------|----------|
| 151 | Reverse Words in a String | Med | 原地反轉 |
| 1071 | Greatest Common Divisor of Strings | Easy | GCD 驗證 |
| 1768 | Merge Strings Alternately | Easy | 交替合併 |

---

## 面試要點

1. **時間和空間的取捨**：何時原地修改，何時建立新字串
2. **字串終止符**：C 中的 '\0' 必須正確處理
3. **邊界條件**：空字串、單字元、相等字串
4. **記憶體管理**：malloc 和 free 的配對
5. **演算法選擇**：為何選擇這種方法而非其他

## 實現檢查清單

- [ ] 正確初始化結果字串的大小
- [ ] 所有邊界索引檢查無誤
- [ ] 字串終止符 '\0' 正確放置
- [ ] 動態分配的記憶體已正確釋放
- [ ] 測試邊界情況（空字串、長度 1）
- [ ] 無緩衝區溢出風險
