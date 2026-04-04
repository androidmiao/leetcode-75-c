# LeetCode 75 C Solutions: Common Programming Idioms Report

**Analysis Date:** April 2026
**Repository:** leetcode-75-c
**Files Analyzed:** 20 representative .c files across array, linked list, tree, graph, DP, and heap problems

---

## Executive Summary

This report identifies and catalogs common C programming idioms, patterns, and techniques used across the LeetCode 75 C solutions. The analysis covers essential low-level patterns that are frequently seen in C interview questions and competitive programming.

---

## 1. Pointer Manipulation Patterns

### 1.1 Fast & Slow Pointers (Two-Pointer Technique)

**Description:** Use two pointers moving at different speeds to detect cycles, find midpoints, or solve two-pointer problems.

**Usage:**
- **Fast Pointer (hare):** Moves 2 steps per iteration
- **Slow Pointer (tortoise):** Moves 1 step per iteration
- Often used in linked list cycle detection and palindrome checking

**Problem Files:**
- `141_Linked List Cycle/141_Linked List Cycle.c` — Floyd's Cycle Detection
- `234-palindrome-linked-list/234_Palindrome Linked List.c` — Find middle of list
- `2095-delete-the-middle-node-of-a-linked-list/solution.c` — Delete middle node

**Code Pattern:**
```c
struct ListNode *slow = head;
struct ListNode *fast = head;

while (fast != NULL && fast->next != NULL) {
    slow = slow->next;           // Move 1 step
    fast = fast->next->next;     // Move 2 steps
}
// slow now at midpoint or cycle detection point
```

---

### 1.2 Two-Pointer Convergence (Cross-traversal)

**Description:** Two pointers start from different ends and traverse toward each other to find intersections or compare elements.

**Usage:**
- Array partition problems (two-sum, container with water)
- Linked list intersection detection
- Symmetric comparisons

**Problem Files:**
- `11-container-with-most-water/solution.c` — Container with Most Water (greedy shrinking)
- `160_Intersection of Two Linked Lists/160_Intersection of Two Linked Lists.c` — Intersection node finding

**Code Pattern:**
```c
int left = 0, right = arr_size - 1;

while (left < right) {
    // Process arr[left] and arr[right]
    if (condition) {
        right--;  // Shrink from right
    } else {
        left++;   // Shrink from left
    }
}
```

---

### 1.3 Pointer-to-Pointer (Double Pointer)

**Description:** Use pointer-to-pointer to modify list structures in-place without returning a new head.

**Usage:**
- Removing nodes from linked lists
- Modifying list pointers during traversal
- Simplifying edge cases

**Problem Files:**
- `206-reverse-linked-list/solution.c` — Linked list reversal (head modification)
- `234-palindrome-linked-list/234_Palindrome Linked List.c` — Reversing list halves

**Code Pattern:**
```c
struct ListNode* curr = head->next;
struct ListNode* next = curr->next;

curr->next = prev;  // Reverse the link
prev = curr;
curr = next;
```

---

### 1.4 Sentinel Node Pattern

**Description:** Use a dummy/sentinel node at the head to eliminate special cases when modifying the first node.

**Usage:**
- Linked list operations where head might change
- Avoiding null pointer checks in critical sections
- Simplifying deletion/insertion logic

**Problem Files:**
- `146_LRU Cache/146_LRU Cache.c` — Doubly-linked list with head/tail sentinels
- `LRU implementation` — Simplifies insertion/removal near boundaries

**Code Pattern:**
```c
typedef struct {
    int key, val;
    struct Node *prev, *next;
} Node;

// Create sentinel head and tail nodes
Node *head = (Node *)malloc(sizeof(Node));  // Head sentinel
Node *tail = (Node *)malloc(sizeof(Node));  // Tail sentinel
head->next = tail;
tail->prev = head;

// Now operations don't need special cases for boundaries
```

---

## 2. Memory Management Patterns

### 2.1 Stack-Allocated Arrays (Fixed-Size)

**Description:** Allocate arrays on the stack for known, bounded sizes to avoid malloc overhead.

**Usage:**
- Fixed-capacity data structures (e.g., 26-letter frequency arrays)
- Local working arrays in functions
- When max size is compile-time constant

**Problem Files:**
- `242_Valid Anagram/Valid Anagram.c` — 26-element frequency array
- `155_Min Stack/Min Stack.c` — Min stack with preallocated capacity
- Frequency counters for character/digit problems

**Code Pattern:**
```c
#define MAX_SIZE 1000

int freq[26];  // For 26 letters, stack-allocated
memset(freq, 0, sizeof(freq));

// Or variable-length array (VLA)
int arr[n];  // Stack-allocated based on runtime n (modern C99)
```

---

### 2.2 Malloc with Calloc for Zero-Initialization

**Description:** Use `calloc` instead of `malloc` when zero-initialization is needed, avoiding explicit memset.

**Usage:**
- Initializing boolean arrays for visited tracking
- Zero-initializing result arrays
- Frequency/count arrays that start at zero

**Problem Files:**
- `739_Daily Temperatures/Daily Temperatures.c` — `calloc` for result array
- `841_Keys and Rooms/841_Keys and Rooms.c` — `calloc` for visited array
- General pattern for allocation with initialization

**Code Pattern:**
```c
// calloc automatically zeros the memory
int *visited = (int *)calloc(n, sizeof(int));  // All zeros
int *result = (int *)calloc(size, sizeof(int));  // Preinitialize to 0

// vs malloc (no zero-init)
int *data = (int *)malloc(n * sizeof(int));  // Uninitialized
```

---

### 2.3 Reusable Heap Allocation with Resize

**Description:** Allocate heap memory with potential resizing when exact capacity is unknown.

**Usage:**
- Dynamic stacks/queues
- Growing arrays
- Flexible data structures

**Problem Files:**
- `155_Min Stack/Min Stack.c` — Stack with capacity tracking
- `703_Kth Largest Element in a Stream/...c` — Heap allocation with fixed size k
- Queue/deque implementations

**Code Pattern:**
```c
typedef struct {
    int *data;
    int capacity;
    int size;
    int top;  // or front/rear for queues
} Stack;

Stack *s = (Stack *)malloc(sizeof(Stack));
s->capacity = 1000;
s->data = (int *)malloc(s->capacity * sizeof(int));
s->top = -1;
```

---

## 3. Comparator Patterns

### 3.1 Custom Comparators for qsort

**Description:** Define comparison functions for `qsort` to enable custom sorting.

**Usage:**
- Sorting arrays by custom criteria
- Frequency-based sorting
- Multi-key sorting (first by count, then by string)

**Problem Files:**
- `692_Top K Frequent Words/692_Top K Frequent Words.c` — Comparator for heap operations (alternative to qsort)

**Code Pattern:**
```c
// Comparator for qsort: return negative/zero/positive
static int compare(const void *a, const void *b) {
    int val_a = *(int *)a;
    int val_b = *(int *)b;

    if (val_a != val_b) {
        return val_a - val_b;  // Ascending
    }
    return val_b - val_a;  // Tie-breaker (descending)
}

// Usage
qsort(arr, n, sizeof(int), compare);
```

---

### 3.2 Heap Comparator (Min-Heap vs Max-Heap)

**Description:** Custom comparison logic embedded in manual heap implementation or heapify operations.

**Usage:**
- Maintaining priority queues
- Top-K problems with min-heaps
- Frequency-based heaps with secondary sorting

**Problem Files:**
- `703_Kth Largest Element in a Stream/...c` — Min-heap for kth largest
- `692_Top K Frequent Words/692_Top K Frequent Words.c` — Min-heap comparator (word count + lexicographic)

**Code Pattern:**
```c
// Min-heap comparator: smallest element at root
static int minHeapCompare(const WordEntry *a, const WordEntry *b) {
    // Lower count = higher priority (comes first in heap)
    if (a->count != b->count) {
        return a->count - b->count;
    }
    // Tie-breaker: lexicographically larger string (reverse alphabetical)
    return strcmp(b->word, a->word);
}

// Usage in heapify down
void heapifyDown(WordEntry *heap, int size, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < size && minHeapCompare(&heap[left], &heap[smallest]) < 0) {
        smallest = left;
    }
    if (right < size && minHeapCompare(&heap[right], &heap[smallest]) < 0) {
        smallest = right;
    }

    if (smallest != idx) {
        swap(&heap[idx], &heap[smallest]);
        heapifyDown(heap, size, smallest);
    }
}
```

---

## 4. Macro Usage Patterns

### 4.1 Min/Max Macros and Inline Functions

**Description:** Define reusable min/max operations via macros or inline functions.

**Usage:**
- Avoiding repetitive comparison code
- Cleaner ternary expressions
- Multi-argument comparisons

**Problem Files:**
- `152_Maximum Product Subarray/Maximum Product Subarray.c` — `max3` and `min3` inline functions
- `53_Maximum Subarray/Maximum Subarray.c` — Ternary operator usage (equivalent)

**Code Pattern:**
```c
// Inline function (cleaner than macro)
static inline int max3(int a, int b, int c) {
    int m = (a > b) ? a : b;
    return (m > c) ? m : c;
}

static inline int min3(int a, int b, int c) {
    int m = (a < b) ? a : b;
    return (m < c) ? m : c;
}

// Usage
int result = max3(current, prev + current, prev * current);
```

---

### 4.2 Hash Size Definition (Prime Numbers)

**Description:** Define hash table sizes as constants, often prime numbers to reduce collisions.

**Usage:**
- Hash table implementations
- Bucket allocation for separate chaining
- Consistent sizing across the codebase

**Problem Files:**
- `146_LRU Cache/146_LRU Cache.c` — `#define HASH_SIZE 10007` (prime)
- `692_Top K Frequent Words/692_Top K Frequent Words.c` — `#define HASH_BUCKETS 1024`

**Code Pattern:**
```c
#define HASH_SIZE 10007  // Prime number chosen for good distribution

// In hash function
static int hash(int key) {
    return ((key % HASH_SIZE) + HASH_SIZE) % HASH_SIZE;
}
```

---

## 5. Data Structure Implementations

### 5.1 Manual Hash Map (Separate Chaining)

**Description:** Implement hash tables using arrays of linked lists (bucket-based collision handling).

**Usage:**
- LRU cache key lookups
- Word frequency counting
- General key-value storage without external libraries

**Problem Files:**
- `146_LRU Cache/146_LRU Cache.c` — Full hash table with separate chaining
- `692_Top K Frequent Words/692_Top K Frequent Words.c` — Word count hash table

**Code Pattern:**
```c
#define HASH_SIZE 10007

typedef struct HashNode {
    int key;
    Node *node;             // Value (can be pointer to actual data)
    struct HashNode *next;  // Collision chain
} HashNode;

typedef struct {
    int capacity;
    HashNode *table[HASH_SIZE];  // Array of bucket heads
} HashTable;

// Lookup
static Node* hashFind(HashTable *ht, int key) {
    int h = hash(key);
    HashNode *curr = ht->table[h];

    while (curr != NULL) {
        if (curr->key == key) {
            return curr->node;
        }
        curr = curr->next;
    }
    return NULL;
}

// Insert
static void hashInsert(HashTable *ht, int key, Node *node) {
    int h = hash(key);
    HashNode *newNode = (HashNode *)malloc(sizeof(HashNode));
    newNode->key = key;
    newNode->node = node;
    newNode->next = ht->table[h];
    ht->table[h] = newNode;
}
```

---

### 5.2 Stack Implementation (Array-Based)

**Description:** Implement stacks using fixed-size arrays with a `top` index.

**Usage:**
- Expression evaluation
- Backtracking (implicit call stack simulation)
- Monotonic stack for next-greater-element problems
- DFS traversal

**Problem Files:**
- `155_Min Stack/Min Stack.c` — Stack with parallel min tracking
- `739_Daily Temperatures/Daily Temperatures.c` — Monotonic decreasing stack
- `841_Keys and Rooms/841_Keys and Rooms.c` — Non-recursive DFS using explicit stack

**Code Pattern:**
```c
typedef struct {
    int *data;
    int top;      // -1 = empty, points to next push location
    int capacity;
} Stack;

void push(Stack *s, int val) {
    if (s->top + 1 >= s->capacity) {
        // Handle resize
        s->capacity *= 2;
        s->data = (int *)realloc(s->data, s->capacity * sizeof(int));
    }
    s->data[++s->top] = val;
}

int pop(Stack *s) {
    if (s->top < 0) return -1;  // Error case
    return s->data[s->top--];
}

int top(Stack *s) {
    return s->top >= 0 ? s->data[s->top] : -1;
}
```

---

### 5.3 Queue Implementation (Circular Array)

**Description:** Implement queues using circular arrays with `front` and `rear` pointers.

**Usage:**
- BFS traversal
- FIFO operations where order matters
- Sliding window problems

**Problem Files:**
- `933_Number of Recent Calls/933_Number of Recent Calls.c` — Simple queue with front/rear

**Code Pattern:**
```c
typedef struct {
    int data[MAX_SIZE];
    int front;  // Points to first element
    int rear;   // Points to next write location
} Queue;

Queue* queueCreate() {
    Queue *q = (Queue *)malloc(sizeof(Queue));
    q->front = 0;
    q->rear = 0;
    return q;
}

void enqueue(Queue *q, int val) {
    q->data[q->rear++] = val;
}

int dequeue(Queue *q) {
    if (q->front >= q->rear) return -1;  // Empty
    return q->data[q->front++];
}

int size(Queue *q) {
    return q->rear - q->front;
}
```

---

### 5.4 Min-Heap Implementation

**Description:** Implement binary min-heaps with array-based storage and heapify operations.

**Usage:**
- Priority queues
- Kth-largest problems
- Heap sort

**Problem Files:**
- `703_Kth Largest Element in a Stream/...c` — Min-heap for finding kth largest
- `692_Top K Frequent Words/692_Top K Frequent Words.c` — Min-heap of size k

**Code Pattern:**
```c
typedef struct {
    int *heap;
    int size;
    int capacity;
} MinHeap;

// Heapify Down: restore heap property after removal from root
void heapifyDown(int *heap, int size, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < size && heap[left] < heap[smallest]) {
        smallest = left;
    }
    if (right < size && heap[right] < heap[smallest]) {
        smallest = right;
    }

    if (smallest != idx) {
        // Swap
        int temp = heap[idx];
        heap[idx] = heap[smallest];
        heap[smallest] = temp;

        heapifyDown(heap, size, smallest);
    }
}

// Heapify Up: restore heap property after insertion
void heapifyUp(int *heap, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (heap[idx] < heap[parent]) {
            int temp = heap[idx];
            heap[idx] = heap[parent];
            heap[parent] = temp;
            idx = parent;
        } else {
            break;
        }
    }
}
```

---

### 5.5 Doubly-Linked List with Sentinel Nodes

**Description:** Use sentinel nodes to simplify boundary conditions in doubly-linked lists.

**Usage:**
- LRU cache implementation
- Any doubly-linked list operation
- Moving/removing nodes safely

**Problem Files:**
- `146_LRU Cache/146_LRU Cache.c` — Full doubly-linked list with head/tail sentinels

**Code Pattern:**
```c
typedef struct Node {
    int key, val;
    struct Node *prev, *next;
} Node;

typedef struct {
    int capacity;
    Node *head, *tail;  // Sentinels
} DLL;

// Initialize with sentinels
void initDLL(DLL *dll, int cap) {
    dll->capacity = cap;
    dll->head = (Node *)malloc(sizeof(Node));
    dll->tail = (Node *)malloc(sizeof(Node));
    dll->head->next = dll->tail;
    dll->tail->prev = dll->head;
}

// Insert before tail (most recently used)
void insertBeforeTail(DLL *dll, Node *node) {
    node->prev = dll->tail->prev;
    node->next = dll->tail;
    dll->tail->prev->next = node;
    dll->tail->prev = node;
}

// Remove node from list
void removeNode(Node *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

// Move node to most-recent position
void moveToTail(DLL *dll, Node *node) {
    removeNode(node);
    insertBeforeTail(dll, node);
}
```

---

## 6. Common C Interview Tricks & Optimizations

### 6.1 XOR Swap (Avoiding Temporary Variable)

**Description:** Use XOR operation to swap two values without a temporary variable.

**Usage:**
- Memory-constrained environments
- Bit manipulation problems
- More for theoretical interest; readability usually preferred

**Note:** Not heavily used in LeetCode 75 C, but a classic interview trick.

**Code Pattern:**
```c
// XOR swap (classic but avoid for readability)
a = a ^ b;
b = a ^ b;
a = a ^ b;

// Better: temporary variable (more readable)
int temp = a;
a = b;
b = temp;
```

---

### 6.2 Bit Manipulation for State Tracking

**Description:** Use individual bits to represent boolean state or set membership.

**Usage:**
- Visited tracking in small graphs (node count ≤ 30)
- Bitmask DP
- Memory optimization

**Problem Files:**
- Not heavily used in current set, but relevant for graph problems with small node counts

**Code Pattern:**
```c
// Bitmask visited tracking (for small graphs, n ≤ 30)
int visited = 0;  // 32-bit integer, each bit = one node

// Mark node i as visited
visited |= (1 << i);

// Check if node i is visited
if (visited & (1 << i)) { ... }

// Unmark
visited &= ~(1 << i);
```

---

### 6.3 Prefix/Suffix Arrays for O(1) Range Queries

**Description:** Precompute cumulative sums or products to answer range queries in O(1).

**Usage:**
- Maximum subarray sum
- Product of array except self
- Cumulative frequency queries

**Problem Files:**
- `238-product-of-array-except-self/solution.c` — Two-pass prefix/suffix product
- `53_Maximum Subarray/Maximum Subarray.c` — Kadane's algorithm (dynamic cumulative)

**Code Pattern:**
```c
// Product of array except self: two-pass approach
int *left = (int *)malloc(n * sizeof(int));
int *right = (int *)malloc(n * sizeof(int));

// Left pass: product of all elements to the left
left[0] = 1;
for (int i = 1; i < n; i++) {
    left[i] = left[i-1] * nums[i-1];
}

// Right pass: product of all elements to the right
right[n-1] = 1;
for (int i = n-2; i >= 0; i--) {
    right[i] = right[i+1] * nums[i+1];
}

// Combine
for (int i = 0; i < n; i++) {
    result[i] = left[i] * right[i];
}
```

---

### 6.4 Ternary Operator for Conditional Assignment

**Description:** Use `condition ? value_true : value_false` for concise conditional logic.

**Usage:**
- Inline min/max operations
- Concise null checks
- State-based returns

**Problem Files:**
- `160_Intersection of Two Linked Lists/...c` — `pA = (pA == NULL) ? headB : pA->next`
- `11-container-with-most-water/solution.c` — Area calculation and direction selection

**Code Pattern:**
```c
// Ternary in null checking
pA = (pA == NULL) ? headB : pA->next;

// Ternary in conditionals
direction = (height[left] > height[right]) ? "right" : "left";

// Nested (though readability may suffer)
int result = (a > b) ? (a > c ? a : c) : (b > c ? b : c);
```

---

### 6.5 Early Return and Guard Clauses

**Description:** Check for edge cases at the start and return early to reduce nesting.

**Usage:**
- Handle empty inputs
- Boundary conditions
- Fail-fast pattern

**Problem Files:**
- `206-reverse-linked-list/solution.c` — Early return for NULL or empty list
- `2095-delete-the-middle-node-of-a-linked-list/solution.c` — Early null check

**Code Pattern:**
```c
int function(struct ListNode *head) {
    // Guard clauses: fail fast on edge cases
    if (!head) return NULL;
    if (!head->next) return head;

    // Main logic only runs if preconditions met
    // ... rest of function
}
```

---

### 6.6 Recursive Tree Traversal with Base Case at Top

**Description:** Check null/base conditions immediately to avoid complex recursion.

**Usage:**
- Binary tree operations
- DFS on graphs
- Clean recursive implementations

**Problem Files:**
- `104_Maximum Depth of Binary Tree/...c` — Immediate null check returning 0
- `133_Clone Graph/Clone Graph.c` — Null check + memoization check at start

**Code Pattern:**
```c
int maxDepth(struct TreeNode *root) {
    // Base case at top
    if (root == NULL) {
        return 0;
    }

    // Recursive case
    int leftDepth = maxDepth(root->left);
    int rightDepth = maxDepth(root->right);

    return (leftDepth > rightDepth ? leftDepth : rightDepth) + 1;
}
```

---

## 7. String and Character Processing

### 7.1 Static Mapping Arrays for Character Lookup

**Description:** Use static arrays indexed by characters to map values.

**Usage:**
- Phone number to letters mapping
- Frequency counting
- Quick lookups by character code

**Problem Files:**
- `17_Letter Combinations of a Phone Number/...c` — `PHONE_MAP[10]` static array
- `242_Valid Anagram/Valid Anagram.c` — `freq[26]` for letter counting

**Code Pattern:**
```c
// Phone mapping
static const char *PHONE_MAP[] = {
    "",     // 0
    "",     // 1
    "abc",  // 2
    "def",  // 3
    "ghi",  // 4
    // ... up to 9
};

// Character to index
int digit = char_digit - '0';
const char *letters = PHONE_MAP[digit];

// Frequency array for 26 letters
int freq[26];
memset(freq, 0, sizeof(freq));

// Count letter 'a'
freq['a' - 'a']++;
```

---

### 7.2 String Copying and Length Tracking

**Description:** Careful string handling with length tracking to avoid buffer overflow.

**Usage:**
- Backtracking with path building
- String comparisons
- Dynamic string allocation

**Problem Files:**
- `17_Letter Combinations of a Phone Number/...c` — `strcpy` for path building in backtracking

**Code Pattern:**
```c
// Safe string operations with length awareness
char path[digitsSize + 1];  // +1 for null terminator
path[index] = letters[i];
path[index + 1] = '\0';

// Copy to result
result[resultIndex] = (char *)malloc((digitsSize + 1) * sizeof(char));
strcpy(result[resultIndex], path);
```

---

## 8. Frequency Counting Patterns

### 8.1 Array-Based Frequency Counter

**Description:** Use fixed-size arrays for character or small-integer frequency counting.

**Usage:**
- Anagram checking
- Character frequency analysis
- Most frequent element finding

**Problem Files:**
- `242_Valid Anagram/Valid Anagram.c` — 26-element char frequency array
- `692_Top K Frequent Words/692_Top K Frequent Words.c` — Hash-based word frequency

**Code Pattern:**
```c
// For lowercase letters only (26 chars)
int freq[26] = {0};

for (int i = 0; s[i] != '\0'; i++) {
    freq[s[i] - 'a']++;
}

for (int i = 0; t[i] != '\0'; i++) {
    freq[t[i] - 'a']--;
}

// Check all zeros
for (int i = 0; i < 26; i++) {
    if (freq[i] != 0) return false;
}
```

---

## 9. Backtracking Pattern

### 9.1 Recursive Backtracking with Path Building

**Description:** Use recursion with a path array/string to explore all combinations.

**Usage:**
- Permutations and combinations
- Letter combinations
- Puzzle solving

**Problem Files:**
- `17_Letter Combinations of a Phone Number/...c` — Full backtracking example

**Code Pattern:**
```c
static void backtrack(const char *digits, int index, int size,
                      char *path, char **result, int *returnSize) {
    // Base case: complete path
    if (index == size) {
        result[*returnSize] = (char *)malloc((size + 1) * sizeof(char));
        strcpy(result[*returnSize], path);
        (*returnSize)++;
        return;
    }

    // Recursive case: try each option
    char digit = digits[index];
    int d = digit - '0';
    const char *letters = PHONE_MAP[d];

    for (int i = 0; i < strlen(letters); i++) {
        path[index] = letters[i];  // Choose
        backtrack(digits, index + 1, size, path, result, returnSize);  // Explore
        // Implicit undo (path gets overwritten)
    }
}
```

---

## 10. Dynamic Programming Patterns

### 10.1 Kadane's Algorithm (Maximum Subarray Sum)

**Description:** Single-pass algorithm tracking current sum and global maximum.

**Usage:**
- Maximum subarray problem
- Generalized to longest increasing subsequence variants

**Problem Files:**
- `53_Maximum Subarray/Maximum Subarray.c` — Classic Kadane's implementation

**Code Pattern:**
```c
int maxSubarray(int *nums, int n) {
    int max_sum = nums[0];
    int current_sum = nums[0];

    for (int i = 1; i < n; i++) {
        // Decide: extend current subarray or start new
        current_sum = (nums[i] > current_sum + nums[i])
                      ? nums[i]
                      : current_sum + nums[i];

        // Update global max
        max_sum = (current_sum > max_sum) ? current_sum : max_sum;
    }

    return max_sum;
}
```

---

### 10.2 Tracking Multiple States (Max and Min for Product)

**Description:** Track both maximum and minimum values when sign changes matter.

**Usage:**
- Maximum product subarray
- Any problem where sign inversion changes extrema

**Problem Files:**
- `152_Maximum Product Subarray/Maximum Product Subarray.c` — Tracks both max_prod and min_prod

**Code Pattern:**
```c
int max_prod = nums[0];
int min_prod = nums[0];
int result = nums[0];

for (int i = 1; i < n; i++) {
    // Store old max before overwriting
    int temp_max = max_prod;

    // Calculate new extrema from three candidates
    max_prod = max3(nums[i], temp_max * nums[i], min_prod * nums[i]);
    min_prod = min3(nums[i], temp_max * nums[i], min_prod * nums[i]);

    result = (max_prod > result) ? max_prod : result;
}
```

---

## 11. Graph Traversal Patterns

### 11.1 Non-Recursive DFS with Explicit Stack

**Description:** Simulate recursion using an explicit stack array instead of call stack.

**Usage:**
- Graph traversal when recursion depth is a concern
- Visiting all reachable nodes
- DFS without managing recursion stack

**Problem Files:**
- `841_Keys and Rooms/841_Keys and Rooms.c` — Non-recursive DFS for room traversal
- `133_Clone Graph/Clone Graph.c` — DFS-based graph cloning

**Code Pattern:**
```c
int *stack = (int *)malloc(n * sizeof(int));
bool *visited = (bool *)calloc(n, sizeof(bool));
int top = 0;

// Start with root
stack[top++] = 0;
visited[0] = true;
int count = 1;

while (top > 0) {
    int node = stack[--top];

    // Process neighbors
    for (int i = 0; i < numNeighbors[node]; i++) {
        int neighbor = neighbors[node][i];
        if (!visited[neighbor]) {
            visited[neighbor] = true;
            stack[top++] = neighbor;
            count++;
        }
    }
}
```

---

### 11.2 Memoization in Recursion

**Description:** Cache results of recursive calls to avoid recomputation.

**Usage:**
- Graph cloning with cycle handling
- Fibonacci-type problems
- Expensive recursive computations

**Problem Files:**
- `133_Clone Graph/Clone Graph.c` — Global `visited[101]` array as memoization

**Code Pattern:**
```c
static struct Node* visited[101];  // Memoization: visited[node_val] = cloned_node

static struct Node* dfs(struct Node* node) {
    if (node == NULL) return NULL;

    // Check memo
    if (visited[node->val] != NULL) {
        return visited[node->val];
    }

    // Compute and store
    struct Node* clone = (struct Node*)malloc(sizeof(struct Node));
    clone->val = node->val;
    visited[node->val] = clone;  // Memoize early to handle cycles

    // Recursively clone neighbors
    // ...

    return clone;
}
```

---

## 12. Implementation Patterns Summary

| Idiom | When to Use | Complexity | Space |
|-------|------------|-----------|-------|
| Two-pointer fast/slow | Linked list cycle, palindrome | O(n) | O(1) |
| Two-pointer convergence | Container max area, two-sum | O(n) | O(1) |
| Sentinel nodes | Linked list modifications | O(n) | O(n) |
| Stack (array-based) | DFS, monotonic problems | O(n) | O(n) |
| Queue (circular) | BFS, sliding window | O(n) | O(n) |
| Min-heap | Kth largest, priority queue | O(n log k) | O(k) |
| Hash table (chaining) | Key-value storage | O(1) avg | O(n) |
| Prefix/suffix arrays | Range queries | O(n) | O(n) |
| Backtracking | Combinations, permutations | Exponential | O(depth) |
| Kadane's algorithm | Max subarray | O(n) | O(1) |
| DFS memoization | Graph cloning, cycle detection | O(n+e) | O(n) |

---

## Key Takeaways

1. **Pointer mastery is essential**: Fast/slow pointers, two-pointer techniques, and pointer-to-pointer patterns appear repeatedly.

2. **Memory management is careful**: Prefer `calloc` for zero-initialization, stack arrays for fixed sizes, and proper malloc/free pairs.

3. **Sentinel nodes simplify logic**: Dummy head/tail nodes eliminate boundary cases in linked list operations.

4. **Array-based structures are common**: Stacks, queues, and heaps implemented directly with arrays, not dynamic structures.

5. **Hash tables are ubiquitous**: Separate chaining is the standard collision resolution in manual implementations.

6. **String/character processing uses static arrays**: Mapping tables and frequency counters indexed directly by character.

7. **DP tracks multiple states**: When sign matters (product problems), track both max and min simultaneously.

8. **Recursion with memoization handles cycles**: Global arrays cache results to prevent infinite recursion.

9. **Explicit stacks simulate DFS**: Non-recursive DFS with manual stack management avoids call-stack limitations.

10. **Ternary operators and guard clauses keep code concise**: Early returns and inline conditionals are standard style.

---

**End of Report**
