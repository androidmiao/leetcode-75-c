Overview
In this problem, we are given a matrix maze that represents a maze of walls and empty cells. We start from the starting point entrance and want to reach the exit of this maze.

An exit is an empty cell located at the border of maze.

img

As shown in the picture above, the cells colored in green are exits because they are empty and at the border of maze. Note that the cell (1, 0) in example 2 is not an exit because it is an entrance: entrance does not count as an exit.

Here our task is to find out the number of steps to the nearest exit in the given maze.

(In the first example above, we move up by 1 step and reach an exit, so the number of steps is 1, while in the second example we need to move two cells right to reach the only exit, thus the number of steps is 2.)

Approach: Breadth First Search (BFS)
Intuition
This problem is about finding the shortest path in a matrix, thus Breadth First Search (BFS) is a promising method.

Why BFS over Depth First Search (DFS) for this problem?

The reason is that DFS is not guaranteed to find the shortest path, as it will explore the matrix as much as possible before moving on to another branch. As shown in the picture below, we may explore the matrix along the green or orange paths first, but these are not the shortest path.

In BFS, however, we explore cells by the order of their distance from the starting position, so whenever we reach an exit cell, we are guaranteed that it is the closest exit!

img

In BFS, we explore cells in the order of their distance from the starting position. We will first visit the cell with a distance of 0, then move on to all the cells with a distance of 1, then move on to all the cells with a distance of 2, and so forth.

img

We use a queue as the container to store all the cells to be visited. Since the operation on a queue is done in First In, First Out (FIFO) order, it allows us to explore all the cells with distance d which we previously stored, before moving on to cells with larger distance d + 1!

How do we prevent revisiting the same cells?

Upon finding an unvisited neighbor cell, we mark it as visited before adding it to the queue, and we skip these visited cells during further searches. Thus, each empty cell will be added to the queue at most once. (Since the input matrix maze use different characters to separate empty cells (.) and walls (+), we can take advantage of this by marking cells to be visited as +.)

Let's take a look at the following slides as an example:

Current


Algorithm
Initialize an empty queue queue to store all the nodes to be visited.
Add entrance and its distance 0 to queue and mark entrance as visited.
While we don't reach an exit and queue still has cells, pop the first cell from queue. Suppose its distance from entrance is curr_distance. We check its neighboring cells in all four directions, if it has an unvisited neighbor cell:
If this neighbor cell is an exit, return its distance from the starting position, curr_distance + 1, as the nearest distance.
Otherwise, we mark it as visited, and add it to queue along with its distance curr_distance + 1.
If we finish the iteration and no exit is found, return -1.
Implementation

Complexity Analysis
Let m,n be the size of the input matrix maze.

Time complexity: O(m⋅n)

For each visited cell, we add it to queue and pop it from queue once, which takes constant time as the operation on queue requires O(1) time.
For each cell in queue, we mark it as visited in maze, and check if it has any unvisited neighbors in all four directions. This also takes constant time.
In the worst-case scenario, we may have to visit O(m⋅n) cells before the iteration stops.
To sum up, the overall time complexity is O(m⋅n).
Space complexity: O(max(m,n))

We modify the input matrix maze in-place to mark each visited cell, it requires constant space.
We use a queue queue to store the cells to be visited. In the worst-case scenario, there may be O(m+n) cells stored in queue.
The space complexity is O(m+n)+O(max(m,n)).

---

概述
在這個問題中，我們會得到一個矩陣 `maze`，它代表一個由牆壁和空格組成的迷宮。我們從起點 `entrance` 出發，目標是走到這個迷宮的出口。

出口是位於迷宮邊界上的空格。

img

如上圖所示，綠色的格子是出口，因為它們是空格，而且位於迷宮邊界。請注意，範例 2 中的 `(1, 0)` 並不是出口，因為它是入口：入口本身不算出口。

我們的任務是找出從入口到最近出口所需的步數。

（在上面的第一個範例中，我們往上走 1 步就到達出口，所以步數是 1；而在第二個範例中，我們需要往右走兩格才會到達唯一的出口，因此步數是 2。）

方法：廣度優先搜尋（BFS）
直覺
這題是在矩陣中尋找最短路徑，因此廣度優先搜尋（BFS）是很適合的方法。

為什麼這題要用 BFS 而不是 DFS？

原因是 DFS 並不能保證找到最短路徑，因為它會沿著某一條分支一路往下探索到底，才會再回頭探索其他分支。如下面圖片所示，我們可能會先沿著綠色路徑或橘色路徑探索，但它們不一定是最短路徑。

相較之下，BFS 會依照與起點距離的順序來探索格子，所以當我們第一次到達某個出口時，可以保證它就是最近的出口！

img

在 BFS 中，我們會按照與起點距離的順序來探索格子。我們會先拜訪距離為 0 的格子，接著是所有距離為 1 的格子，再接著是所有距離為 2 的格子，依此類推。

img

我們使用 queue 作為儲存所有待拜訪格子的容器。由於 queue 採用先進先出（FIFO, First In, First Out）的操作順序，因此它能讓我們先完整探索所有距離為 `d` 的格子，再去探索距離更大的 `d + 1` 的格子。

我們要如何避免重複拜訪同一個格子？

當找到一個尚未拜訪過的鄰居格子時，我們會先把它標記為已拜訪，再把它加入 queue；之後搜尋時便會跳過這些已拜訪的格子。因此，每個空格最多只會被加入 queue 一次。（由於輸入矩陣 `maze` 已經用不同字元區分空格 `.` 與牆壁 `+`，我們可以利用這一點，直接把準備拜訪的格子標記成 `+`。）

讓我們透過下面的投影片看一個例子：

Current


演算法
初始化一個空的 queue，用來儲存所有待拜訪的節點。
把 `entrance` 以及它的距離 `0` 加入 queue，並將 `entrance` 標記為已拜訪。
當我們還沒找到出口，而且 queue 裡還有格子時，從 queue 中取出最前面的格子。假設它到 `entrance` 的距離是 `curr_distance`。接著檢查它在四個方向上的鄰居格子，如果有尚未拜訪的鄰居格子：
如果這個鄰居格子是出口，就回傳它距離起點的距離 `curr_distance + 1`，作為最近出口的距離。
否則，我們就把它標記為已拜訪，並把它和距離 `curr_distance + 1` 一起加入 queue。
如果整個迭代結束後仍然找不到出口，就回傳 `-1`。
實作

複雜度分析
令 `m, n` 為輸入矩陣 `maze` 的大小。

時間複雜度：`O(m⋅n)`

對於每個被拜訪的格子，我們都會把它加入 queue 一次，並從 queue 中取出一次，而 queue 的這些操作都只需要 `O(1)` 時間。
對於 queue 中的每個格子，我們會在 `maze` 中把它標記為已拜訪，並檢查它在四個方向上是否有尚未拜訪的鄰居。這同樣只需要常數時間。
在最壞情況下，整個迭代停止前，我們可能需要拜訪 `O(m⋅n)` 個格子。
總結來說，整體時間複雜度是 `O(m⋅n)`。
空間複雜度：`O(max(m,n))`

我們直接在輸入矩陣 `maze` 上原地修改，把每個拜訪過的格子標記起來，因此這部分只需要常數空間。
我們使用 queue 來儲存待拜訪的格子。在最壞情況下，queue 中可能會同時存放 `O(m+n)` 個格子。
空間複雜度是 `O(m+n)+O(max(m,n))`。
