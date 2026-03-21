// https://leetcode.com/problems/top-k-frequent-words/description/?envType=problem-list-v2&envId=wl94y6ih
/*
 * 692. Top K Frequent Words
 * LeetCode function signature:
 *     char** topKFrequent(char** words, int wordsSize, int k, int* returnSize)
 *
 * Google interview 取向版本：
 * 使用 editorial 的 Approach 3: Min Heap
 *
 * 為什麼面試會偏好這個版本：
 * 1. 題目的 follow-up 明確要求 O(n log k) time。
 * 2. Min heap of size k 是 top-k 題型非常常見的標準解法。
 * 3. 你可以先講 brute force / sorting 當 baseline，
 *    再自然優化成 min heap，這很符合面試對 tradeoff 的期待。
 *
 * 這份實作分成兩大階段：
 *
 *   階段 1：用 hash table 統計每個單字的頻率
 *   階段 2：用大小固定為 k 的 min heap 維護前 k 名
 *
 * ASCII 圖解：
 *
 *   words
 *   ["i","love","leetcode","i","love","coding"]
 *                  |
 *                  v
 *   hash table 計數
 *   "i"        -> 2
 *   "love"     -> 2
 *   "leetcode" -> 1
 *   "coding"   -> 1
 *                  |
 *                  v
 *   逐一丟進 size = k 的 min heap
 *
 *   heap 規則：heap top 放「目前前 k 名裡最差的那個」
 *   所謂最差是：
 *   1. 頻率較小
 *   2. 若頻率相同，字典序較大
 *
 *   例如把目前候選想成：
 *   ("i", 2), ("love", 2), ("coding", 1)
 *
 *   在最終答案裡，好的順序是：
 *   ("i", 2) 比 ("love", 2) 好，因為字典序較小
 *   ("love", 2) 比 ("coding", 1) 好，因為頻率較高
 *
 *   但在 min heap 裡，我們故意把「比較差」的元素放上面：
 *
 *              ("love", 2)   <- top，因為它比 ("i", 2) 差
 *              /
 *         ("i", 2)
 *
 *   這樣如果有更好的新候選進來，就只要拿它和 heap top 比較即可。
 *
 *   例如 k = 2
 *
 *   目前 heap:
 *      top -> ("love", 2)
 *             ("i", 2)
 *
 *   如果新候選是 ("coding", 1)
 *   因為它比 top 還差，所以直接忽略
 *
 *   如果新候選比 top 更好
 *   就把 top 換掉，再重新維護 heap
 *
 *                  |
 *                  v
 *   最後 heap 中留下的，就是前 k 名
 *   但 pop 出來會是「由差到好」
 *   所以答案要從尾巴往前填，才能得到：
 *   ["i", "love"]
 *
 * 時間複雜度：
 * - 頻率統計：平均 O(n)
 * - 每個不同單字進 heap：O(log k)
 * - 總時間複雜度：平均 O(n log k)
 *
 * 空間複雜度：
 * - hash table + entries：O(n)
 * - heap：O(k)
 * - 總額外空間：O(n)
 */

#include <stdlib.h>
#include <string.h>

#define HASH_BUCKETS 1024

/* 記錄一個不同單字，以及它出現的次數。 */
typedef struct {
    char* word;  // 指向原始輸入中的字串
    int count;   // 該字串出現次數
} WordEntry;

/* hash table 的鏈結節點：key 對應到 entries 裡的某個 index。 */
typedef struct HashNode {
    char* key;               // 單字本身
    int entryIndex;          // 對應到 entries[entryIndex]
    struct HashNode* next;   // separate chaining 用的 next 指標
} HashNode;

/* djb2 hash：簡單、常見，而且很適合面試講解。 */
static unsigned int hashWord(const char* word) {
    unsigned long hash = 5381;  // djb2 的經典初始值

    /* 逐字元把字串混進 hash。 */
    while (*word != '\0') {
        /* hash * 33 + current_char */
        hash = ((hash << 5) + hash) + (unsigned char)(*word);
        word++;  // 換處理下一個字元
    }

    /* 壓到 bucket 範圍內。 */
    return (unsigned int)(hash % HASH_BUCKETS);
}

/*
 * 最終答案排序規則：
 * 1. 頻率高的比較好
 * 2. 若頻率相同，字典序小的比較好
 *
 * 回傳值 < 0 代表 left 應該排在 right 前面。
 */
static int compareForAnswer(const WordEntry* left, const WordEntry* right) {
    /* 先比 count，count 大的在答案中要排前面。 */
    if (left->count != right->count) {
        return (left->count > right->count) ? -1 : 1;
    }

    /* count 一樣時，字典序較小者排前面。 */
    return strcmp(left->word, right->word);
}

/*
 * Min heap 的排序規則要反過來思考：
 * heap top 應該放「目前前 k 名裡最差的那個」。
 *
 * 也就是：
 * 1. 頻率較小的比較差，應該更靠近 heap top
 * 2. 若頻率相同，字典序較大的比較差，也應該更靠近 heap top
 *
 * 回傳值 < 0 代表 left 應該排在 right 前面，也就是 left 更差。
 */
static int compareForHeap(const WordEntry* left, const WordEntry* right) {
    /* 在 min heap 中，count 小的比較差，所以要更靠近 root。 */
    if (left->count != right->count) {
        return (left->count < right->count) ? -1 : 1;
    }

    /*
     * count 一樣時，字典序較大的比較差。
     * 例如：
     * ("love", 2) 比 ("i", 2) 差
     * 因為最終答案要求字典序小的排前面。
     */
    return strcmp(right->word, left->word);
}

/* 判斷 candidate 是否比 current 更值得留在 top k。 */
static int isBetter(const WordEntry* candidate, const WordEntry* current) {
    /* 若 candidate 在最終答案排序中應排在 current 前面，代表它更好。 */
    return compareForAnswer(candidate, current) < 0;
}

/* 交換 heap 內兩個節點指標。 */
static void swapEntryPointer(WordEntry** left, WordEntry** right) {
    WordEntry* temp = *left;  // 暫存左邊
    *left = *right;           // 右邊移到左邊
    *right = temp;            // 暫存值放回右邊
}

/* 從 index 往上調整，維持 min heap 性質。 */
static void siftUp(WordEntry** heap, int index) {
    /* 只要不是 root，就持續和 parent 比較。 */
    while (index > 0) {
        int parent = (index - 1) / 2;  // 完全二元樹中 parent 的位置

        /* 如果目前節點沒有比 parent 更差，就不用再往上浮。 */
        if (compareForHeap(heap[index], heap[parent]) >= 0) {
            break;
        }

        /* 目前節點比較差，應該浮到更上面。 */
        swapEntryPointer(&heap[index], &heap[parent]);
        index = parent;  // 繼續檢查新的 parent
    }
}

/* 從 index 往下調整，維持 min heap 性質。 */
static void siftDown(WordEntry** heap, int index, int heapSize) {
    while (1) {
        int leftChild = index * 2 + 1;   // 左子節點
        int rightChild = index * 2 + 2;  // 右子節點
        int smallest = index;            // 先假設目前節點最差

        /* 如果左子節點更差，就改記錄左子節點。 */
        if (leftChild < heapSize && compareForHeap(heap[leftChild], heap[smallest]) < 0) {
            smallest = leftChild;
        }

        /* 如果右子節點更差，就改記錄右子節點。 */
        if (rightChild < heapSize && compareForHeap(heap[rightChild], heap[smallest]) < 0) {
            smallest = rightChild;
        }

        /* 如果最差的還是自己，代表 heap 性質已成立。 */
        if (smallest == index) {
            break;
        }

        /* 否則把更差的孩子換上來。 */
        swapEntryPointer(&heap[index], &heap[smallest]);
        index = smallest;  // 繼續往下修正
    }
}

/* 把一個候選放進 size 最多為 k 的 min heap。 */
static void pushTopKHeap(WordEntry** heap, int* heapSize, int limit, WordEntry* candidate) {
    /* heap 還沒滿，直接放到尾巴再往上調整。 */
    if (*heapSize < limit) {
        heap[*heapSize] = candidate;  // 新元素先放在最後面
        siftUp(heap, *heapSize);      // 再往上浮到正確位置
        (*heapSize)++;                // heap 內元素數量加一
        return;
    }

    /*
     * heap[0] 是目前 top k 裡最差的元素。
     * 如果新候選更好，就把 heap top 換掉。
     */
    if (isBetter(candidate, heap[0])) {
        heap[0] = candidate;             // 直接覆蓋最差元素
        siftDown(heap, 0, *heapSize);    // 從 root 往下修正
    }
}

/* 從 min heap 取出 heap top。 */
static WordEntry* popHeap(WordEntry** heap, int* heapSize) {
    WordEntry* top = heap[0];  // 先記住 root，等等要回傳

    (*heapSize)--;  // 邏輯上先把 heap 大小減一

    if (*heapSize > 0) {
        heap[0] = heap[*heapSize];   // 把最後一個元素搬到 root
        siftDown(heap, 0, *heapSize);  // 再往下修正 heap
    }

    return top;
}

/* 釋放整張 hash table 的鏈結節點。 */
static void freeHashTable(HashNode** table) {
    /* 每個 bucket 都可能是一條 linked list，要一條一條釋放。 */
    for (int i = 0; i < HASH_BUCKETS; i++) {
        HashNode* current = table[i];  // 從這個 bucket 的表頭開始

        while (current != NULL) {
            HashNode* next = current->next;  // 先記住下一個節點
            free(current);                   // 釋放目前節點
            current = next;                  // 繼續往後走
        }
    }
}

char** topKFrequent(char** words, int wordsSize, int k, int* returnSize) {
    HashNode* table[HASH_BUCKETS] = {0};  // hash table buckets，初始全為 NULL
    WordEntry* entries;                   // 所有不同單字與頻率
    WordEntry** heap;                     // size = k 的 min heap
    char** answer;                        // LeetCode 回傳答案
    int uniqueCount = 0;                  // 不同單字總數
    int heapSize = 0;                     // 目前 heap 裡有幾個元素

    /* 先把 returnSize 預設成 0，避免呼叫端拿到垃圾值。 */
    if (returnSize != NULL) {
        *returnSize = 0;  // 先設預設值，避免提早 return 時留下未定義內容
    }

    /* 保守處理異常輸入。 */
    if (words == NULL || wordsSize <= 0 || k <= 0) {
        return NULL;
    }

    /* 最多不會超過 wordsSize 個不同單字，所以先開到最大。 */
    entries = (WordEntry*)malloc((size_t)wordsSize * sizeof(WordEntry));  // 最多 wordsSize 個不同單字
    if (entries == NULL) {
        return NULL;
    }

    /* heap 最多只需要放 k 個元素。 */
    heap = (WordEntry**)malloc((size_t)k * sizeof(WordEntry*));  // heap 只存指向 entry 的指標
    if (heap == NULL) {
        free(entries);
        return NULL;
    }

    /*
     * 階段 1：hash table 統計頻率
     *
     * 做法：
     * - 先用 hash 找 bucket
     * - 在 bucket 的 linked list 中找是否已存在相同單字
     * - 找到就 count++
     * - 沒找到就新增一個 WordEntry 與一個 HashNode
     */
    for (int i = 0; i < wordsSize; i++) {
        unsigned int bucket = hashWord(words[i]);  // 算出這個字該落在哪個 bucket
        HashNode* current = table[bucket];         // 從 bucket 鏈結串列開頭開始找

        /* 先看 hash table 裡有沒有這個字。 */
        while (current != NULL) {
            if (strcmp(current->key, words[i]) == 0) {
                entries[current->entryIndex].count++;  // 找到了，直接把頻率加一
                break;                                 // 不用再往下找
            }

            current = current->next;  // 沒找到，繼續掃下一個節點
        }

        /* current == NULL 代表整條鏈都沒這個字，需要新增。 */
        if (current == NULL) {
            HashNode* newNode = (HashNode*)malloc(sizeof(HashNode));

            if (newNode == NULL) {
                freeHashTable(table);
                free(heap);
                free(entries);
                return NULL;
            }

            entries[uniqueCount].word = words[i];   // 記錄新單字
            entries[uniqueCount].count = 1;         // 第一次出現，頻率為 1

            newNode->key = words[i];                // hash table key
            newNode->entryIndex = uniqueCount;      // 對應 entries 的位置
            newNode->next = table[bucket];          // 頭插法：掛到目前 bucket 前面
            table[bucket] = newNode;                // 更新 bucket 表頭

            uniqueCount++;                          // 不同單字數量加一
        }
    }

    /*
     * 階段 2：把所有不同單字丟進 size = k 的 min heap。
     *
     * heap 只保留目前最好的 k 個；
     * root 永遠是這 k 個裡面最差的那個。
     */
    for (int i = 0; i < uniqueCount; i++) {
        pushTopKHeap(heap, &heapSize, k, &entries[i]);  // 嘗試把這個 entry 納入前 k 名
    }

    /* 配置回傳答案陣列。 */
    answer = (char**)malloc((size_t)k * sizeof(char*));
    if (answer == NULL) {
        freeHashTable(table);
        free(heap);
        free(entries);
        return NULL;
    }

    /*
     * min heap 每次 pop 出來的是「由差到好」，
     * 所以答案要從尾巴往前填，才能得到最終正確順序。
     */
    for (int i = k - 1; i >= 0; i--) {
        answer[i] = popHeap(heap, &heapSize)->word;  // 由右往左填，抵消 min heap 的「由差到好」順序
    }

    if (returnSize != NULL) {
        *returnSize = k;  // 明確告訴 LeetCode：答案共有 k 個字串
    }

    /* 清理中間資料結構。 */
    freeHashTable(table);
    free(heap);
    free(entries);

    return answer;  // 回傳最終答案陣列
}
