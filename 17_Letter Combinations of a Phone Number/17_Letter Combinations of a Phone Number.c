//Backtracking: https://leetcode.com/problems/letter-combinations-of-a-phone-number/description/?envType=study-plan-v2&envId=leetcode-75

#include <stdlib.h>
#include <string.h>

/*
題目圖解說明
==============================
輸入範例：digits = "23"

PHONE_MAP 對應如下：
2 -> "abc"
3 -> "def"

整體目標：
把第 1 個數字能選的字母，和第 2 個數字能選的字母，全部做排列組合。

回溯樹如下：

                    ""
          /           |           \
         a            b            c
       / | \        / | \        / | \
      d  e  f      d  e  f      d  e  f
      |  |  |      |  |  |      |  |  |
     ad ae af     bd be bf     cd ce cf

程式實際在做的事：
1. 先從 index = 0 開始，處理 digits[0] = '2'
2. 從 "abc" 中依序挑一個字母放進 path
3. 再往下一層遞迴，處理 digits[1] = '3'
4. 從 "def" 中依序挑一個字母接到 path 後面
5. 當 path 長度等於 digits 長度時，代表得到一組完整答案
6. 把這組答案複製到 result 中
7. 回到上一層，改選下一個字母，直到所有分支都走完

以 path 的變化來看：

開始：path = ""
選到 'a'：path = "a"
再選到 'd'：path = "ad"   -> 存入答案
回上一層，改選 'e'：path = "ae" -> 存入答案
回上一層，改選 'f'：path = "af" -> 存入答案
回到第一層，改選 'b'，重複同樣流程
最後得到：
["ad", "ae", "af", "bd", "be", "bf", "cd", "ce", "cf"]

這就是標準的回溯法：
「做選擇 -> 進入下一層 -> 完成後返回 -> 改做下一個選擇」
*/

// 定義數字對應到英文字母的表，索引 0 到 9 分別對應字元 '0' 到 '9'
static const char *PHONE_MAP[] = {
    "",     // 0 沒有對應字母
    "",     // 1 沒有對應字母
    "abc",  // 2 對應 abc
    "def",  // 3 對應 def
    "ghi",  // 4 對應 ghi
    "jkl",  // 5 對應 jkl
    "mno",  // 6 對應 mno
    "pqrs", // 7 對應 pqrs
    "tuv",  // 8 對應 tuv
    "wxyz"  // 9 對應 wxyz
};

// 使用回溯法遞迴產生所有字母組合
// 參數說明：
// digits      : 原始輸入數字字串，例如 "23"
// index       : 目前正在處理第幾個數字
// digitsSize  : 輸入字串總長度
// path        : 目前已經組好的字母路徑
// result      : 儲存所有答案的字串陣列
// returnSize  : 目前已產生幾組答案
static void backtrack(const char *digits, int index, int digitsSize,
                      char *path, char **result, int *returnSize) {
    // 如果目前已經處理完所有數字，代表 path 內是一組完整答案
    if (index == digitsSize) {
        // 配置一塊新記憶體，長度為字串長度加上結尾字元 '\0'
        result[*returnSize] = (char *)malloc((digitsSize + 1) * sizeof(char));
        // 把目前組合複製到答案陣列中
        strcpy(result[*returnSize], path);
        // 答案數量加 1，準備存放下一個組合
        (*returnSize)++;
        // 已完成一組答案，直接返回上一層
        return;
    }

    // 取得目前數字字元，例如 '2'
    char digitChar = digits[index];
    // 把字元數字轉成整數索引，例如 '2' 轉成 2
    int digit = digitChar - '0';
    // 找出這個數字所對應的所有字母
    const char *letters = PHONE_MAP[digit];
    // 取得目前可選字母的數量
    int lettersCount = (int)strlen(letters);

    // 逐一嘗試目前數字可以對應的每一個字母
    for (int i = 0; i < lettersCount; i++) {
        // 把目前選擇的字母放到 path 的對應位置
        path[index] = letters[i];
        // 補上字串結尾，讓 path 在任何時候都是合法 C 字串
        path[index + 1] = '\0';
        // 遞迴處理下一個數字
        backtrack(digits, index + 1, digitsSize, path, result, returnSize);
    }
}

/*
函式流程圖
==============================
letterCombinations("23")
        |
        v
計算總組合數 = 3 * 3 = 9
        |
        v
建立 result[9] 與 path
        |
        v
backtrack(digits, 0, 2, path, result, returnSize)
        |
        +--> 處理 digits[0] = '2'，可選 a / b / c
                |
                +--> path = "a"
                |       |
                |       +--> 處理 digits[1] = '3'，可選 d / e / f
                |               |
                |               +--> "ad" 存入
                |               +--> "ae" 存入
                |               +--> "af" 存入
                |
                +--> path = "b"
                |       |
                |       +--> "bd"、"be"、"bf" 存入
                |
                +--> path = "c"
                        |
                        +--> "cd"、"ce"、"cf" 存入
*/
char **letterCombinations(char *digits, int *returnSize) {
    // 先把回傳的答案數量初始化為 0
    *returnSize = 0;

    // 如果輸入為空字串，依照題意回傳空陣列
    if (digits == NULL || digits[0] == '\0') {
        // LeetCode 接受回傳 NULL 表示沒有任何結果
        return NULL;
    }

    // 取得輸入數字字串的長度
    int digitsSize = (int)strlen(digits);
    // 預估最多組合數，初始值設為 1，之後逐位相乘
    int totalCombinations = 1;

    // 計算總共有多少種組合，好先配置答案陣列大小
    for (int i = 0; i < digitsSize; i++) {
        // 取得目前數字對應的字母數量
        int digit = digits[i] - '0';
        // 總組合數乘上這個數字可對應的字母數量
        totalCombinations *= (int)strlen(PHONE_MAP[digit]);
    }

    // 配置答案陣列，每個元素都會指向一個字串答案
    char **result = (char **)malloc(totalCombinations * sizeof(char *));
    // 配置目前路徑字串，長度需要包含結尾字元 '\0'
    char *path = (char *)malloc((digitsSize + 1) * sizeof(char));
    // 先把 path 清空成空字串
    path[0] = '\0';

    // 從第 0 個數字開始進行回溯搜尋
    backtrack(digits, 0, digitsSize, path, result, returnSize);
    // path 只是暫存用，所有答案都已另外複製，這裡可以釋放
    free(path);

    // 回傳所有字母組合
    return result;
}
