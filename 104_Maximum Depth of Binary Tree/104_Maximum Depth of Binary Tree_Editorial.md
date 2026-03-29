Solution
Tree definition

First of all, here is the definition of the TreeNode which we would use.



Approach 1: Recursion
Intuition
By definition, the maximum depth of a binary tree is the maximum number of steps to reach a leaf node from the root node.

From the definition, an intuitive idea would be to traverse the tree and record the maximum depth during the traversal.

Algorithm

Current

One could traverse the tree either by Depth-First Search (DFS) strategy or Breadth-First Search (BFS) strategy.
For this problem, either way would do.
Here we demonstrate a solution that is implemented with the DFS strategy and recursion.


Complexity analysis

Time complexity: we visit each node exactly once, thus the time complexity is O(N), where N is the number of nodes.

Space complexity: in the worst case, the tree is completely unbalanced, e.g. each node has only left child node, the recursion call would occur N times (the height of the tree), therefore the storage to keep the call stack would be O(N). But in the best case (the tree is completely balanced), the height of the tree would be log(N). Therefore, the space complexity in this case would be O(log(N)).



Approach 2: Tail Recursion + BFS
One might have noticed that the above recursion solution is probably not the most optimal one in terms of space complexity, and in the extreme case, the overhead of the call stack might even lead to stack overflow.

To address the issue, one can tweak the solution a bit to make it tail recursion, which is a specific form of recursion where the recursive call is the last action in the function.

The benefit of having tail recursion is that for certain programming languages (e.g. C++) the compiler could optimize the memory allocation of the call stack by reusing the same space for every recursive call, rather than creating the space for each one. As a result, one could obtain the constant space complexity O(1) for the overhead of the recursive calls.

Here is a sample solution. Note that the optimization of tail recursion is not supported by Python or Java.


Complexity analysis

Time complexity: O(N), still we visit each node once and only once.

Space complexity: O(2 
(log 
2
​
 N−1)
 )=O(N/2)=O(N), i.e. the maximum number of nodes at the same level (the number of leaf nodes in a full binary tree), since we traverse the tree in the BFS manner.

As one can see, this probably is not the best example to apply the tail recursion technique. Because though we did gain the constant space complexity for the recursive calls, we pay the price of O(N) complexity to maintain the state information for recursive calls. This defeats the purpose of applying tail recursion.

However, we would like to stress the point that the tail recursion is a useful form of recursion that could eliminate the space overhead incurred by the recursive function calls.

Note: a function cannot be tail recursion if there are multiple occurrences of recursive calls in the function, even if the last action is the recursive call. Because the system has to maintain the function call stack for the sub-function calls that occur within the same function.




Approach 3: Iteration
Intuition

We could also convert the above recursion into iteration, with the help of the stack data structure. Similar to the behaviors of the function call stack, the stack data structure follows the pattern of FILO (First-In-Last-Out), i.e. the last element that is added to a stack would come out first.

With the help of the stack data structure, one could mimic the behaviors of the function call stack that is involved in the recursion, to convert a recursive function to a function with iteration.

Algorithm

The idea is to keep the next nodes to visit in a stack.
Due to the FILO behavior of the stack, one would get the order of visits the same as the one in recursion.

We start from a stack that contains the root node and the corresponding depth which is 1. Then we proceed to the iterations: pop the current node out of the stack and push the child nodes. The depth is updated at each step.


Complexity analysis

Time complexity: O(N).

Space complexity: in the worst case, the tree is completely unbalanced, e.g. each node has only the left child node, the recursion call would occur N times (the height of the tree), the storage to keep the call stack would be O(N). But in the average case (the tree is balanced), the height of the tree would be log(N). Therefore, the space complexity in this case would be O(log(N)).

---
解法
樹的定義

首先，這裡是我們會使用的 `TreeNode` 定義。



方法 1：遞迴
直覺
根據定義，二元樹的最大深度就是從根節點走到葉節點所需的最大步數。

由這個定義出發，一個直觀的想法是遍歷整棵樹，並在遍歷過程中記錄最大深度。

演算法

目前

我們可以用深度優先搜尋（DFS）或廣度優先搜尋（BFS）來遍歷這棵樹。
對這一題來說，兩種方式都可行。
這裡我們示範的是使用 DFS 策略搭配遞迴實作的解法。


複雜度分析

時間複雜度：我們恰好會拜訪每個節點一次，因此時間複雜度是 `O(N)`，其中 `N` 是節點數量。

空間複雜度：在最差情況下，樹完全不平衡，例如每個節點都只有左子節點，此時遞迴呼叫會發生 `N` 次（也就是樹的高度），因此用來保存呼叫堆疊的空間會是 `O(N)`。但在最佳情況下（樹完全平衡），樹高會是 `log(N)`。因此在這種情況下，空間複雜度會是 `O(log(N))`。



方法 2：尾遞迴 + BFS
你可能已經注意到，上面的遞迴解法在空間複雜度方面不一定是最理想的；在極端情況下，呼叫堆疊的額外負擔甚至可能造成 stack overflow。

為了解決這個問題，我們可以稍微調整解法，讓它變成尾遞迴。尾遞迴是一種特殊形式的遞迴，特點是遞迴呼叫是函式中的最後一個動作。

尾遞迴的好處是，對某些程式語言（例如 C++）來說，編譯器可能會最佳化呼叫堆疊的記憶體配置，讓每一次遞迴呼叫重複使用相同空間，而不是每次都建立新的空間。因此，遞迴呼叫本身的額外空間開銷有可能降到常數空間 `O(1)`。

下面是一個範例解法。請注意，Python 和 Java 並不支援尾遞迴最佳化。


複雜度分析

時間複雜度：`O(N)`，因為我們仍然只會拜訪每個節點一次。

空間複雜度：`O(2(log₂N−1)) = O(N/2) = O(N)`，也就是同一層中節點數量的最大值（在滿二元樹中，也就是葉節點數量），因為我們是以 BFS 的方式遍歷樹。

正如你所看到的，這可能不是套用尾遞迴技巧的最佳例子。雖然我們確實獲得了遞迴呼叫本身的常數空間開銷，但卻付出了 `O(N)` 的空間來維護遞迴呼叫所需的狀態資訊，這反而抵銷了使用尾遞迴的目的。

不過，我們仍然想強調，尾遞迴是一種很有用的遞迴形式，能夠消除一般遞迴函式呼叫所帶來的額外空間負擔。

注意：如果一個函式中出現多次遞迴呼叫，那它就不能算是尾遞迴，即使最後一個動作是遞迴呼叫也一樣。因為系統仍然必須保留呼叫堆疊，以處理同一函式內其他子函式的呼叫。




方法 3：迭代
直覺

我們也可以藉由堆疊（stack）資料結構，把上面的遞迴解法改寫成迭代版本。和函式呼叫堆疊的行為類似，stack 遵循 FILO（First-In-Last-Out，先進後出）的模式，也就是最後放進 stack 的元素會最先被取出。

藉由 stack 資料結構，我們可以模擬遞迴時函式呼叫堆疊的行為，從而把遞迴函式轉換成迭代函式。

演算法

這個想法是把下一個要拜訪的節點保存在 stack 中。
由於 stack 具有 FILO 特性，我們取得節點的拜訪順序會和遞迴時相同。

一開始，stack 內放入根節點以及其對應深度 `1`。接著開始迭代：把目前節點從 stack 彈出，再把它的子節點壓入 stack。深度會在每一步中更新。


複雜度分析

時間複雜度：`O(N)`。

空間複雜度：在最差情況下，樹完全不平衡，例如每個節點都只有左子節點，此時遞迴呼叫會發生 `N` 次（也就是樹高），保存呼叫堆疊所需的空間會是 `O(N)`。但在平均情況下（樹是平衡的），樹高會是 `log(N)`。因此在這種情況下，空間複雜度會是 `O(log(N))`。
