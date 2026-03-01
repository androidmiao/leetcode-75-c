//Array/String: https://leetcode.com/problems/merge-strings-alternately/?envType=study-plan-v2&envId=leetcode-75
//這支程式的功能是把兩個字串「交錯合併」，例如 abc 和 pqr 變成 apbqcr
//Hint: Use two pointers, one pointer for each string. Alternately choose the character from each pointer, and move the pointer upwards.

/* 
圖解流程:
假設：
- `word1 = "abc"`
- `word2 = "pqrs"`

初始化：
```text
i=0, j=0, k=0
res = [ _ _ _ _ _ _ _ ]   (長度 7 = 3+4)
```

每輪 `while (i<n1 || j<n2)` 都做：
1) 若 `i<n1`，放 `word1[i]`
2) 若 `j<n2`，放 `word2[j]`

---

### 第1輪
```text
放 word1[i]='a' -> res[k]
res = [ a _ _ _ _ _ _ ]   i=1, j=0, k=1

放 word2[j]='p' -> res[k]
res = [ a p _ _ _ _ _ ]   i=1, j=1, k=2
```

### 第2輪
```text
放 word1[i]='b'
res = [ a p b _ _ _ _ ]   i=2, j=1, k=3

放 word2[j]='q'
res = [ a p b q _ _ _ ]   i=2, j=2, k=4
```

### 第3輪
```text
放 word1[i]='c'
res = [ a p b q c _ _ ]   i=3, j=2, k=5

放 word2[j]='r'
res = [ a p b q c r _ ]   i=3, j=3, k=6
```

### 第4輪（word1 用完）
```text
i<n1 ? 3<3 -> 否（跳過）
j<n2 ? 3<4 -> 是，放 's'
res = [ a p b q c r s ]   i=3, j=4, k=7
```

結束後補 `'\0'`：
```text
res[7] = '\0'
結果字串 = "apbqcrs"
```

---

@核心圖（交錯 + 補尾）：
```text
word1: a   b   c
word2:   p   q   r   s
res  : a p b q c r s
```
*/


/* 
複雜度:
時間：O(n1 + n2)（每個字元只處理一次）
空間：O(n1 + n2)（輸出字串大小）
*/

/*
以下是「記憶體位址/陣列索引版」圖解（以 `word1="abc"`, `word2="pqrs"`）：

```text
word1 (char[])
index:   0   1   2   3
value:  'a''b''c''\0'

word2 (char[])
index:   0   1   2   3   4
value:  'p''q''r''s''\0'
```

`malloc(n1+n2+1)` 配出 `res[8]`（索引 0~7）：

```text
res (heap, char[8])
index:   0   1   2   3   4   5   6   7
value:  [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
```

初始指標/索引：
```text
i=0 -> word1[0]
j=0 -> word2[0]
k=0 -> res[0]
```

第1輪後：
```text
res[k]=word1[i] => res[0]='a' ; i=1,k=1
res[k]=word2[j] => res[1]='p' ; j=1,k=2

res:
index:   0   1   2   3   4   5   6   7
value:  'a''p' [ ] [ ] [ ] [ ] [ ] [ ]
```

第2輪後：
```text
res[2]='b' ; i=2,k=3
res[3]='q' ; j=2,k=4

res:
index:   0   1   2   3   4   5   6   7
value:  'a''p''b''q' [ ] [ ] [ ] [ ]
```

第3輪後：
```text
res[4]='c' ; i=3,k=5
res[5]='r' ; j=3,k=6

res:
index:   0   1   2   3   4   5   6   7
value:  'a''p''b''q''c''r' [ ] [ ]
```

第4輪（word1 已越界，僅取 word2）：
```text
i<n1 ? 3<3 -> false
j<n2 ? 3<4 -> true: res[6]='s' ; j=4,k=7
```

最後補字串結尾：
```text
res[7] = '\0'
```

@最終 `res`：
```text
index:   0   1   2   3   4   5   6    7
value:  'a''p''b''q''c''r''s''\0'
string: "apbqcrs"
```

指標位置結束狀態：
```text
i=3 (==n1), j=4 (==n2), k=7
```
*/

//這支程式的功能是把兩個字串「交錯合併」，例如 abc 和 pqr 變成 apbqcr
//#include 區塊
#include <stdlib.h> //供 malloc 使用
#include <string.h> //供 strlen 使用

//函式宣告: 改成使用 size_t（長度與索引）並加入 malloc 失敗檢查，接著快速編譯確認沒有語法問題。
char* mergeAlternately(char* word1, char* word2) { //輸入兩個 C 字串，回傳新配置的結果字串（heap 記憶體）
    //計算長度
    size_t n1 = strlen(word1); //n1, n2 分別存 word1, word2 長度
    size_t n2 = strlen(word2);

    //配置結果空間
    char *res = malloc(n1 + n2 + 1); //配置 n1 + n2 + 1 bytes：n1+n2 放實際字元，+1 放結尾 '\0'
    //檢查 malloc 是否失敗
    if (res == NULL) return NULL;
    //三個索引變數：i, j 分別指向 word1, word2 的當前字元位置，k 指向 res 的當前字元位置
    size_t i = 0, j = 0, k = 0;

    //主要合併迴圈
    while (i < n1 || j < n2) { //只要任一字串還有字元就繼續
        //這種寫法自然處理長短不一：短的用完後，長的會持續被追加到尾端
        if (i < n1) res[k++] = word1[i++]; //如果 word1 還有字元，從 word1 取一個字元放到 res，i 和 k 都加 1 (若 word1 還有字元，放進 res，並遞增 i 和 k)
        if (j < n2) res[k++] = word2[j++]; //如果 word2 還有字元，從 word2 取一個字元放到 res，j 和 k 都加 1 (若 word2 還有字元，放進 res，並遞增 j 和 k)
    }

    //結尾與回傳
    res[k] = '\0'; //補 '\0' 讓它成為合法 C 字串
    return res; //回傳結果指標; 回傳的字串由呼叫端負責 free()
}


/*
下面限制條件會不會影響目前這個程式設計？
- `1 <= word1.length, word2.length <= 100`
- `word1` 與 `word2` 都只包含英文小寫字母。

--> 影響很小，基本上**不影響你現在這個設計的正確性**。
- `1 <= length <= 100`  
  代表長度很小，`int` 當索引沒問題，也不會有記憶體大小溢位風險。  
  你的寫法即使遇到空字串其實也能運作，但題目保證不會是空字串。

- 只包含英文小寫字母  
  對這題演算法幾乎沒差，因為你只是逐字元搬運，不做字元分類或轉換。  
  換成其他可列印字元，這份邏輯也一樣可用。

但如果長度大於100呢?
也能正常跑，演算法本身不依賴 `100` 這個上限。

會受影響的是實務上的「型別與記憶體安全」：
- 你現在用 `int n1, n2` 接 `strlen`，超大字串時可能有轉型風險。
- 更穩健做法是改用 `size_t`（`strlen` 的回傳型別）來存長度和索引。
- 另外保留 `malloc` 失敗檢查即可。

所以「大於 100」不會改變邏輯正確性，但建議把型別改成 `size_t` 會更通用。

*/

/*
"用 `int n1, n2` 接 `strlen`，超大字串時可能有轉型風險", 請說一下範例:
//當 `strlen` 回傳的字串長度超過 `INT_MAX`（通常是 2,147,483,647）時，使用 `int` 來存儲這個長度會導致溢位，因為 `int` 的最大值被超過了。這會導致 `n1` 或 `n2` 變成負數或不正確的值，進而引發未定義行為。
可以，這裡給你一個具體例子。

假設環境是常見的：
- `size_t` = 64 位元（可表示很大）
- `int` = 32 位元（最大約 `2,147,483,647`）

`strlen()` 回傳型別是 `size_t`。  
如果字串長度是 `3,000,000,000`（30 億）：

```c
size_t real = strlen(word1);   // 3000000000
int n1 = (int)real;            // 超出 int 範圍
```

@這個轉型超出範圍，結果會變成錯誤值（常見是負數或截斷值，屬實作定義）。  
接著程式可能出現：

1. 迴圈條件判斷錯（例如 `i < n1` 一開始就不成立）  
2. 配置大小算錯（`malloc(n1 + n2 + 1)` 得到錯誤大小）  
3. 後續寫入越界，導致崩潰或未定行為

所以長度/索引用 `size_t` 才是安全且對應 `strlen` 的做法。
*/

/*
先做一次快速編譯檢查，確認改動沒有引入錯誤：
gcc -Wall -Wextra -Werror -c 1768-merge-strings-alternately-c/1768-merge-strings-alternately.c -o /tmp/1768-merge-strings-alternately.o

/opt/homebrew/Library/Homebrew/cmd/shellenv.sh: line 18: /bin/ps: Operation not permitted

已依照建議完成修改，並且編譯通過。

變更在 [1768-merge-strings-alternately.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/1768-merge-strings-alternately-c/1768-merge-strings-alternately.c)：
- `n1`, `n2` 從 `int` 改成 `size_t`
- `i`, `j`, `k` 從 `int` 改成 `size_t`
- `malloc` 保留 `NULL` 檢查
- 移除 `malloc` 的不必要型別轉型（C 語言慣例）

驗證：
- 已執行 `gcc -Wall -Wextra -Werror -c ...`，無錯誤。

*/