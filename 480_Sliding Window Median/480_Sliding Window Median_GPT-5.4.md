已完成 C 實作並寫入 480_Sliding Window Median.c。

這份解法用的是「雙堆 + 延遲刪除」：
small 是 max-heap，存較小的一半；large 是 min-heap，存較大的一半。視窗滑動時，離開的元素不一定在堆頂，所以先記到雜湊表，等它浮到堆頂時再真正刪除，整體可做到 O(n log k)。我也把主要資料結構、插入、刪除、平衡、取中位數都加了逐行中文註解。

我已用題目兩個範例和一個偶數視窗案例編譯測試，結果正確：
[1,3,-1,-3,5,3,6,7], k=3 -> [1,-1,-1,3,5,6]
[1,2,3,4,2,3,1,4,2], k=3 -> [2,3,3,3,2,3,2]
[1,4,2,3], k=4 -> [2.5]




概念上是一樣的。你現在的 [480_Sliding Window Median.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/480_Sliding%20Window%20Median/480_Sliding%20Window%20Median.c#L9) 用的就是「雙堆 + 延遲刪除」，對應 [480_Sliding Window Median_Solution.md](/Users/alex.miao/Documents/GitHub/leetcode-75-c/480_Sliding%20Window%20Median/480_Sliding%20Window%20Median_Solution.md#L33) 的 `Approach 2: Two Heaps (Lazy Removal)`。

但它們不完全一樣，差異在實作細節，而且目前 C 版有一個重要風險。`Solution.md` 明確寫到 lazy removal 會讓 heap 的實際大小在某些情況下長到 `O(n)`，不是只維持在 `k` 以內，[480_Sliding Window Median_Solution.md](/Users/alex.miao/Documents/GitHub/leetcode-75-c/480_Sliding%20Window%20Median/480_Sliding%20Window%20Median_Solution.md#L91) 到 [480_Sliding Window Median_Solution.md](/Users/alex.miao/Documents/GitHub/leetcode-75-c/480_Sliding%20Window%20Median/480_Sliding%20Window%20Median_Solution.md#L100) 有講清楚；但 C 版在 [480_Sliding Window Median.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/480_Sliding%20Window%20Median/480_Sliding%20Window%20Median.c#L502) 到 [480_Sliding Window Median.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/480_Sliding%20Window%20Median/480_Sliding%20Window%20Median.c#L505) 把兩個 heap 的容量固定成 `k + 1`。這代表如果遇到遞減之類會累積 stale elements 的資料，heap 可能超出容量，程式就有越界風險。

所以結論是：
目前 C 程式的「思路」和 `Solution.md` 的方法 2 一樣。
但 `Solution.md` 的描述比較完整、比較正確。
如果要比哪個比較好，概念上是同一類；實作品質上，`Solution.md` 那個版本比較可靠，因為它有把 lazy removal 導致 heap 變大的問題講出來，而目前這份 C 版還沒完全處理。

如果你要，我下一步可以直接把 [480_Sliding Window Median.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/480_Sliding%20Window%20Median/480_Sliding%20Window%20Median.c) 修成「heap 可自動擴容」版本，讓它真正和 `Solution.md` 的方法 2 完全一致。




已修成可自動擴容版本，修改在 480_Sliding Window Median.c。

我加了 heapEnsureCapacity()，讓 heapPush() 插入前先檢查容量，不夠就用倍增方式 realloc。這樣即使 lazy deletion 讓 stale elements 暫時堆積，heap 也不會再被 k + 1 的固定容量卡住。dualHeapInit() 的註解也一起改了，現在和 Solution.md 的方法 2 一致。

我已重新編譯並測試：
題目兩個範例都正確，另外也跑了遞減序列這種容易累積 stale elements 的案例，結果正常。