//https://leetcode.com/problems/minimum-window-substring/description/?envType=problem-list-v2&envId=wl94y6ih

#include <limits.h>
#include <stdlib.h>
#include <string.h>

/*
圖解說明：

題目目標：
在字串 s 中，找出一段最短連續區間，使它至少包含 t 的所有字元。

範例：
s = "ADOBECODEBANC"
t = "ABC"

t 需要的字元統計：
A:1  B:1  C:1
required = 3

--------------------------------------------------
步驟 1：先用 right 向右擴張，直到湊齊 A、B、C
--------------------------------------------------

索引：  0 1 2 3 4 5 6 7 8 9 10 11 12
s   ：  A D O B E C O D E B  A  N  C

一開始：
left = 0
right = 0
window = ""

right 往右走，依序吃進字元：

吃到 A 後：
[A]
 L R
required: 3 -> 2

再繼續吃到 B：
[A D O B]
 L     R
required: 2 -> 1

再繼續吃到 C：
[A D O B E C]
 L         R
required: 1 -> 0

此時視窗已合法，可以開始收縮左邊。

--------------------------------------------------
步驟 2：當 required == 0，就讓 left 右移來縮小視窗
--------------------------------------------------

目前：
[A D O B E C]
 L         R

先記錄這是一組可行答案，長度 = 6

然後嘗試移動 left：
移掉 A 之後，A 就不夠了

[D O B E C]
 L       R
required: 0 -> 1

視窗重新變成不合法，所以不能再縮，必須繼續擴張 right。

--------------------------------------------------
步驟 3：再次擴張 right，直到又湊齊所有需求
--------------------------------------------------

right 繼續往右移，最後會形成：
[D O B E C O D E B A N C]
 L                     R

這時又重新包含 A、B、C，required 再次變成 0。

--------------------------------------------------
步驟 4：再縮 left，找出更短合法區間
--------------------------------------------------

持續把左邊不必要的字元丟掉：

[O B E C O D E B A N C]
 L                   R

[B E C O D E B A N C]
 L                 R

[E C O D E B A N C]
 L               R

[C O D E B A N C]
 L             R

再縮到：
[B A N C]
 L     R

這段同時包含 A、B、C，而且長度只有 4，
比前面的長度 6 更短，所以更新最佳答案為 "BANC"。

--------------------------------------------------
核心觀念
--------------------------------------------------

1. right 負責「擴張視窗」，把需要的字元收進來。
2. 當 required == 0，代表目前視窗已經合法。
3. left 負責「收縮視窗」，把多餘字元丟掉，直到不能再丟。
4. 每次合法時都嘗試更新最短答案。

--------------------------------------------------
need[] 陣列的意義
--------------------------------------------------

假設 t = "ABC"
一開始：
need['A'] = 1
need['B'] = 1
need['C'] = 1

當 right 吃進某字元時：
need[ch]--

意義：
- need[ch] > 0 : 這個字元還缺
- need[ch] == 0 : 這個字元剛好夠
- need[ch] < 0 : 這個字元多出來了

當 left 移出某字元時：
need[ch]++

如果加回去後 need[ch] > 0，
表示這個字元又變成缺少，視窗因此失效。
*/

char * minWindow(char * s, char * t) {
    // 宣告兩個長度變數，分別記錄字串 s 與 t 的長度。
    int sLen = (int)strlen(s);
    int tLen = (int)strlen(t);

    // need 用來記錄 t 中每個字元還需要幾個。
    // 題目保證只會有英文字母，因此 128 個 ASCII 位置已足夠使用。
    int need[128] = {0};

    // left 是滑動視窗左邊界，right 是右邊界。
    int left = 0;
    int right = 0;

    // required 表示目前還有多少個 t 的字元尚未被視窗完整涵蓋。
    int required = tLen;

    // minStart 用來記錄目前最短答案的起點。
    int minStart = 0;

    // minLen 用來記錄目前找到的最短視窗長度，初始設成最大值表示尚未找到答案。
    int minLen = INT_MAX;

    // 先統計 t 中每個字元需要出現的次數。
    for (int i = 0; i < tLen; i++) {
        need[(unsigned char)t[i]]++;
    }

    // 開始移動右指標，逐步擴張視窗。
    while (right < sLen) {
        // 取得目前右指標指向的字元。
        unsigned char currentRightChar = (unsigned char)s[right];

        // 如果這個字元仍然是我們需要的，表示成功覆蓋到一個必要字元。
        if (need[currentRightChar] > 0) {
            required--;
        }

        // 不論是否必要，都先把 need 減 1。
        // 如果變成負數，代表這個字元在視窗中是多出來的。
        need[currentRightChar]--;

        // 右指標往右擴張一格。
        right++;

        // 當 required == 0，表示目前視窗已經完整包含 t 的所有字元。
        while (required == 0) {
            // 如果目前視窗更短，就更新最佳答案。
            if (right - left < minLen) {
                minLen = right - left;
                minStart = left;
            }

            // 準備收縮左邊界，所以先取出左邊界字元。
            unsigned char currentLeftChar = (unsigned char)s[left];

            // 左邊界字元即將離開視窗，所以把 need 加回去。
            need[currentLeftChar]++;

            // 如果加回去後 need 大於 0，代表這個字元原本是必要的，
            // 現在少掉它之後，視窗就不再滿足條件了。
            if (need[currentLeftChar] > 0) {
                required++;
            }

            // 左指標右移，嘗試讓視窗更小。
            left++;
        }
    }

    // 如果 minLen 仍然是初始值，代表根本沒有找到合法視窗。
    if (minLen == INT_MAX) {
        // 依照 LeetCode 常見要求，回傳一個動態配置的空字串。
        char *emptyResult = (char *)malloc(1);

        // 如果配置失敗，直接回傳 NULL。
        if (emptyResult == NULL) {
            return NULL;
        }

        // 設定字串結尾字元，形成空字串。
        emptyResult[0] = '\0';
        return emptyResult;
    }

    // 配置答案字串空間，長度需要 minLen 再加上字串結尾 '\0'。
    char *result = (char *)malloc((size_t)minLen + 1);

    // 如果配置失敗，回傳 NULL。
    if (result == NULL) {
        return NULL;
    }

    // 把最短視窗內容從 s 複製到 result。
    memcpy(result, s + minStart, (size_t)minLen);

    // 手動補上字串結尾。
    result[minLen] = '\0';

    // 回傳最終答案。
    return result;
}
