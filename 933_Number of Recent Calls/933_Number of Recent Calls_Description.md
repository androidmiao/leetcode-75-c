You have a RecentCounter class which counts the number of recent requests within a certain time frame.

Implement the RecentCounter class:

RecentCounter() Initializes the counter with zero recent requests.
int ping(int t) Adds a new request at time t, where t represents some time in milliseconds, and returns the number of requests that has happened in the past 3000 milliseconds (including the new request). Specifically, return the number of requests that have happened in the inclusive range [t - 3000, t].
It is guaranteed that every call to ping uses a strictly larger value of t than the previous call.

 

Example 1:

Input
["RecentCounter", "ping", "ping", "ping", "ping"]
[[], [1], [100], [3001], [3002]]
Output
[null, 1, 2, 3, 3]

Explanation
RecentCounter recentCounter = new RecentCounter();
recentCounter.ping(1);     // requests = [1], range is [-2999,1], return 1
recentCounter.ping(100);   // requests = [1, 100], range is [-2900,100], return 2
recentCounter.ping(3001);  // requests = [1, 100, 3001], range is [1,3001], return 3
recentCounter.ping(3002);  // requests = [1, 100, 3001, 3002], range is [2,3002], return 3
 

Constraints:

1 <= t <= 109
Each test case will call ping with strictly increasing values of t.
At most 104 calls will be made to ping.

---

你有一個 `RecentCounter` 類別，用來計算某個時間範圍內最近的請求數量。

請實作 `RecentCounter` 類別：

`RecentCounter()`：使用 0 筆最近請求初始化計數器。  
`int ping(int t)`：在時間 `t` 新增一筆請求，其中 `t` 代表以毫秒為單位的某個時間點，並回傳過去 3000 毫秒內發生的請求數量（包含這筆新請求）。更明確地說，請回傳發生在區間 `[t - 3000, t]`（含端點）內的請求數量。  
保證每次呼叫 `ping` 時使用的 `t` 都會嚴格大於前一次呼叫的 `t`。

範例 1：

輸入
["RecentCounter", "ping", "ping", "ping", "ping"]
[[], [1], [100], [3001], [3002]]
輸出
[null, 1, 2, 3, 3]

說明
`RecentCounter recentCounter = new RecentCounter();`  
`recentCounter.ping(1);`     // requests = [1]，區間為 [-2999,1]，回傳 1  
`recentCounter.ping(100);`   // requests = [1, 100]，區間為 [-2900,100]，回傳 2  
`recentCounter.ping(3001);`  // requests = [1, 100, 3001]，區間為 [1,3001]，回傳 3  
`recentCounter.ping(3002);`  // requests = [1, 100, 3001, 3002]，區間為 [2,3002]，回傳 3

限制條件：

`1 <= t <= 10^9`  
每個測試案例都會以嚴格遞增的 `t` 值呼叫 `ping`。  
最多會呼叫 `ping` `10^4` 次。
