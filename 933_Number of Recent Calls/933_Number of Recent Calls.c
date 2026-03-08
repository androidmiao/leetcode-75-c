//Queue: https://leetcode.com/problems/number-of-recent-calls/?envType=study-plan-v2&envId=leetcode-75
//C 的解法可以用 queue 的概念實作，因為 `t` 保證遞增，所以只要把超過區間 `[t - 3000, t]` 的舊請求移掉即可。

/*
思路很直接：
- `rear` 放新進來的時間
- `front` 往前移除太舊的時間
- 最後 queue 內剩下的數量就是答案

時間複雜度是 `O(1)` amortized (攤銷)，每個元素最多進出一次；
--> 單看某一次 ping()，裡面有 while，看起來像可能很多次。但攤銷來看是 O(1)。
原因是：
每個時間點只會被 rear 加入一次
每個時間點也只會被 front 移過一次
也就是每個元素最多處理兩次，所以總成本是線性的，平均到每次操作就是：Amortized O(1)

空間複雜度是 `O(n)`。因為 queue 需要存放所有尚未被移除的請求時間
*/

/*
用題目的範例跑一次會更清楚。
假設依序呼叫：
ping(1)
ping(100)
ping(3001)
ping(3002)

1. ping(1)
放入 1
合法範圍 [ -2999, 1 ]
queue: [1]
回傳 1

2. ping(100)
放入 100
合法範圍 [ -2900, 100 ]
queue: [1, 100]
都合法
回傳 2

3. ping(3001)
放入 3001
合法範圍 [1, 3001]
queue: [1, 100, 3001]
都合法
回傳 3

4. ping(3002)
放入 3002
合法範圍 [2, 3002]
queue 原本是 [1, 100, 3001, 3002]
1 太舊 < 2，移掉; 100 >= 2，保留
剩下有效的是 [100, 3001, 3002]
回傳 3
不用真的刪除陣列元素, 只要把 front 往前推，就表示前面的資料已經不算了

這個解法為什麼可行，關鍵在這句題目保證：
It is guaranteed that every call to ping uses a strictly larger value of t
也就是每次新的 t 都比前一次大。

這代表：
請求時間天然就是遞增的
queue 裡的資料也會一直保持遞增
因此過期的元素一定只會出現在前面
所以只要從 front 開始移掉就好，不需要在中間找或排序
如果題目沒有保證 t 遞增，這個寫法就不成立。
*/

#include <stdlib.h> //有用到 malloc() 和 free()

// 使用陣列模擬 queue。因為題目保證每次 ping 的 t 都會嚴格遞增，所以過期的請求一定只會出現在 queue 前端。
/*
data:  [1][100][3001][3002][...]
        ^
      front
rear 會指向最後一個元素的下一格
所以目前 queue 內元素個數就是：rear - front
*/
typedef struct {
    int data[10000];  // 儲存每次請求的時間t，題目最多呼叫 10^4 次
    int front;        // 正面: 指向目前 queue 最前面有效元素的位置
    int rear;         // 後部: 指向下一個要放入新資料(可寫入)的位置
} RecentCounter;

RecentCounter* recentCounterCreate() {
    RecentCounter* obj = (RecentCounter*)malloc(sizeof(RecentCounter)); // 配置一個 RecentCounter 結構體的記憶體空間

    // 一開始 queue 是空的，所以 front 和 rear 都從 0 開始
    obj->front = 0;
    obj->rear = 0;

    // 回傳建立好的物件
    return obj;
}

int recentCounterPing(RecentCounter* obj, int t) {
    obj->data[obj->rear++] = t; // 1.先把新的請求時間放到 queue 尾端，再把 rear 往後移動
    /*
    等同於：
    obj->data[obj->rear] = t;
    obj->rear++;
    有效區間: data[front] ~ data[rear - 1]
    Ex: data = [1, 100, 3001]
    front = 0
    rear = 3    
    */    

    // 2.把太舊的請求移出範圍
    // 只保留合法區間 [t - 3000, t] 內的請求;如果最前面的時間小於 t - 3000，代表它已經過期，要移出有效範圍
    while (obj->data[obj->front] < t - 3000) {
        obj->front++;
    }

    // 3.回傳目前 queue 內有效元素(請求)數量 = 尾端位置 - 前端位置
    return obj->rear - obj->front;
}

void recentCounterFree(RecentCounter* obj) {
    // 釋放 recentCounterCreate() 配置的記憶體
    free(obj); //把 malloc 配置的記憶體釋放掉，避免記憶體洩漏
}
