# LeetCode 面試知識庫系統 — Karpathy 方法論的 Google 面試版

> 基於 Karpathy 的 LLM Knowledge Base 方法論，針對你的 `leetcode-75-c` repo + Obsidian + Cowork 工作流設計。
> 目標：讓每次刷題產生的知識自動累積成可查詢、可複習、會自己長大的面試備戰知識庫。

---

## 核心概念：你的 repo 已經走了一半

Karpathy 的方法分六步：收集 → AI 編譯成 wiki → 對 wiki 提問 → 回存答案 → 健康檢查 → 做工具。

你的 repo 現狀：70+ 道題，每道都有 `.c`、`_Description.md`、`_Editorial.md`、`_Linux.md`、`_Memory.md`、`_Opus-4.6.md`。這些就是 Karpathy 說的 `raw/` — 已經是高品質的原始素材。

**缺的是第二步到第五步**：把這些逐題的素材「編譯」成跨題目的模式知識庫，讓知識從「一題一題記」變成「一類一類懂」。

---

## 系統架構

```
leetcode-75-c/
├── wiki/                          ← 新增：AI 編譯的跨題知識庫
│   ├── _index.md                  ← 知識庫總目錄（自動更新）
│   ├── patterns/                  ← 解題模式百科
│   │   ├── two-pointers.md
│   │   ├── sliding-window.md
│   │   ├── monotonic-stack.md
│   │   ├── bfs-dfs.md
│   │   ├── binary-search.md
│   │   ├── dynamic-programming.md
│   │   ├── heap-priority-queue.md
│   │   ├── union-find.md
│   │   ├── trie.md
│   │   └── ...
│   ├── c-idioms/                  ← C 語言面試慣用寫法
│   │   ├── pointer-manipulation.md
│   │   ├── manual-memory.md
│   │   ├── comparator-patterns.md
│   │   └── macro-tricks.md
│   ├── kernel/                    ← Linux Kernel 知識交叉索引
│   │   ├── list-head.md
│   │   ├── rbtree.md
│   │   ├── hash-table.md
│   │   └── ...
│   ├── interview/                 ← 面試實戰知識
│   │   ├── complexity-cheatsheet.md
│   │   ├── edge-cases.md
│   │   ├── follow-up-patterns.md
│   │   └── code-review-questions.md
│   ├── logs/                      ← 查詢回存紀錄
│   │   └── 2026-04-04_session.md
│   └── health/                    ← 健康檢查報告
│       └── 2026-04-04_review.md
├── 1_Two Sum/                     ← 既有的逐題資料夾（raw）
├── 23_Merge k Sorted Lists/       ← 既有的逐題資料夾（raw）
├── ...
└── knowledge-base-system.md       ← 本文件（方法論說明）
```

**關鍵原則**：`wiki/` 裡的內容全部由 AI 產生和維護，你自己的筆記和想法寫在各題的 `_Opus-4.6.md` 裡。兩者分開，但互相連結。

---

## 六步流程：逐步說明

### 第一步：收集（你已經在做了）

每次用 Cowork 初始化一道新題，SKILL.md 流程會自動產生：

| 檔案 | Karpathy 對照 | 性質 |
|------|---------------|------|
| `_Description.md` | 原始資料 | 題目全文 + 繁中翻譯 |
| `_Editorial.md` | 原始資料 | 官方解法筆記 |
| `.c` 解答 | 原始資料 | 你的實作 + 繁中註解 |
| `_Opus-4.6.md` | 你的筆記 | 解法比較、心得 |
| `_Linux.md` | 原始資料 | Kernel 對照分析 |
| `_Memory.md` | 原始資料 | 記憶體圖解 |

**不需要額外動作**。你現有的 Cowork 初始化流程就是收集步驟。

---

### 第二步：AI 編譯 — 把逐題素材變成模式百科

這是整套系統最關鍵的步驟。每完成一批題目（建議每 5-10 題，或每週一次），對 Cowork 下這個指令：

```
請幫我更新 wiki/ 知識庫。掃描所有題目資料夾裡的 .c、_Editorial.md、
_Opus-4.6.md、_Linux.md，然後：
1. 更新 patterns/ 裡的解題模式文件 — 每個模式要包含：
   - 模式定義和辨識信號（什麼時候該用這個模式）
   - 所有相關題目一覽表（題號、難度、變體說明）
   - 模板程式碼（C 語言）
   - 常見陷阱和 edge cases
   - 面試時怎麼講解這個模式（30 秒版和 2 分鐘版）
2. 更新 c-idioms/ — 從所有 .c 檔案中提煉 C 語言慣用寫法
3. 更新 kernel/ — 從所有 _Linux.md 中交叉整理 kernel 知識
4. 更新 interview/ — 整理面試高頻考點和 follow-up 問題
5. 更新 _index.md 目錄
```

#### `wiki/patterns/` 裡每個檔案的結構

以 `two-pointers.md` 為例：

```markdown
# Two Pointers 雙指標模式

## 辨識信號
- 題目涉及排序陣列或鏈結串列
- 需要找一對/多個元素滿足某條件
- 需要原地操作（O(1) space）

## 變體分類

### 對向雙指標（相向而行）
| 題號 | 題目 | 難度 | 關鍵差異 |
|------|------|------|----------|
| 11 | Container With Most Water | Med | 貪心收縮 |
| 167 | Two Sum II | Med | 排序陣列 |
| ... | | | |

### 快慢指標（同向不同速）
| 題號 | 題目 | 難度 | 關鍵差異 |
|------|------|------|----------|
| 141 | Linked List Cycle | Easy | Floyd 環偵測 |
| 234 | Palindrome Linked List | Easy | 找中點 + 反轉 |
| ... | | | |

### 滑動窗口指標（同向同速）
...

## C 語言模板
（從 repo 裡的 .c 檔案提煉的通用寫法）

## 面試講解
### 30 秒版
### 2 分鐘版

## 常見 Follow-up
## 和 Kernel 的關聯
（從 _Linux.md 交叉引用）

## 繁體中文版
（以上全部的繁中翻譯/改寫）
```

---

### 第三步：對知識庫提問

這是知識庫開始回報價值的時候。在 Cowork 裡，你可以這樣使用：

#### 場景 A：遇到新題，先查模式

```
我在做 LeetCode 42 (Trapping Rain Water)。
請查 wiki/patterns/ 判斷這題最可能屬於哪個模式，
然後給我該模式的模板和相關題目，幫我快速切入。
```

#### 場景 B：面試模擬問答

```
假設面試官問我 "為什麼你用 two pointers 而不是 hash map？"
請根據 wiki/patterns/two-pointers.md 和 wiki/interview/ 裡的內容，
幫我準備一個有說服力的回答。
```

#### 場景 C：Kernel 面試加分

```
面試到 LRU Cache 這題時，我想順便展示 Kernel 知識。
請從 wiki/kernel/ 裡找出 LRU 相關的 kernel 實作，
幫我準備一段 30 秒的延伸說明。
```

#### 場景 D：弱點分析

```
看一下 wiki/patterns/ 裡所有模式，
哪些模式我的題目覆蓋率最低？哪些模式只有 Easy 沒有 Medium/Hard？
列出我下一批應該刷的 5 道題。
```

---

### 第四步：查詢結果回存

每次有價值的問答，都存回知識庫：

```
wiki/logs/2026-04-04_session.md
```

格式：

```markdown
# 2026-04-04 學習紀錄

## Q: Two Pointers vs Hash Map 的取捨
（問題）
（回答摘要）
→ 已更新至 wiki/patterns/two-pointers.md 的「面試講解」段落

## Q: LRU Cache 的 Kernel 延伸
（問題）
（回答摘要）
→ 已更新至 wiki/kernel/lru-cache.md
```

**對 Cowork 的指令**：

```
把剛才的問答摘要存到 wiki/logs/ 裡，
並且把有價值的新知識合併回對應的 wiki/ 文件。
```

---

### 第五步：定期健康檢查

每 1-2 週，對 Cowork 下這個指令：

```
請對 wiki/ 做一次健康檢查：
1. 掃描所有 patterns/ 文件，和實際的題目資料夾對比，
   找出還沒被歸類到任何模式的題目
2. 檢查各模式的題目覆蓋率，標出弱點
3. 檢查 _Opus-4.6.md 和 wiki/ 之間有沒有矛盾
4. 列出最近新做的題目帶來了哪些新的 cross-pattern 洞察
5. 產出報告存到 wiki/health/
6. 更新 _index.md
```

健康檢查報告範例：

```markdown
# 知識庫健康檢查 — 2026-04-04

## 覆蓋率摘要
- patterns/ 共 10 個模式，覆蓋 68/75 題 (90.7%)
- 未歸類：#XXX, #YYY, ...

## 弱點警示
- DP 模式只有 3 題，建議補 #XXX, #XXX
- Graph/BFS 缺 Hard 題

## 新洞察
- 最近做的 #215 (Kth Largest) 同時涉及 Heap 和 QuickSelect，
  建議在 patterns/heap.md 裡加入 QuickSelect 比較段落

## 矛盾檢查
- 無矛盾發現

## 下一步建議
1. ...
2. ...
```

---

### 第六步：工具化（進階，非必要）

你的 Cowork + SKILL.md 已經是一個很好的「工具」。如果未來想進一步自動化：

- **Scheduled Task**：用 Cowork 的排程功能，設定每週自動跑健康檢查
- **Obsidian 整合**：在 Obsidian 裡用 `[[wiki/patterns/two-pointers]]` 的雙向連結語法，讓你的個人筆記和 AI wiki 互相引用
- **語音複習**：把 `wiki/patterns/` 裡的模式摘要丟進 ChatGPT 語音模式，通勤時用講的問問題（Lex Fridman 的做法）

---

## 快速啟動：今天就能做的三件事

### 1. 建立 wiki/ 目錄結構

對 Cowork 說：

```
請在 repo 裡建立 wiki/ 目錄結構（patterns/, c-idioms/, kernel/,
interview/, logs/, health/），並建立空的 _index.md。
```

### 2. 第一次全量編譯

對 Cowork 說：

```
請掃描 repo 裡所有題目的 .c 和 _Editorial.md，
按照 knowledge-base-system.md 裡的格式，
產生第一版 wiki/patterns/ 文件。先從 Two Pointers 和
Linked List 這兩個模式開始。
```

### 3. 設定你的 Obsidian 連結

在 Obsidian 裡把 `leetcode-75-c` 當作一個 vault 打開，`wiki/` 裡的所有 `.md` 都會自動變成可搜尋、可連結的筆記。

---

## 和 Karpathy 方法的對照表

| Karpathy 的步驟 | 你的版本 | 執行方式 |
|-----------------|---------|---------|
| `raw/` 資料夾收集 | 每題的 `_Description.md` + `_Editorial.md` + `.c` | Cowork 初始化流程（已有） |
| AI 編譯成 wiki | `wiki/patterns/`, `wiki/kernel/` 等 | 每週對 Cowork 下指令 |
| 對 wiki 提問 | 在 Cowork 裡查模式、模擬面試 | 隨時使用 |
| 回存查詢結果 | `wiki/logs/` + 合併回 wiki 文件 | 每次有價值的問答後 |
| 健康檢查 | `wiki/health/` 報告 | 每 1-2 週一次 |
| 做工具 | SKILL.md + Obsidian 雙向連結 | 已有，可漸進擴充 |

---

## 維護原則

1. **你只負責刷題和問問題** — wiki/ 裡的內容全由 AI 維護
2. **你的想法寫在 `_Opus-4.6.md`** — 個人筆記和 AI wiki 分開
3. **wiki/ 每篇文件都要能追溯回原始題目** — 保持連結完整
4. **知識庫越用越厚** — 每次問答都是在餵養知識庫
5. **不追求完美，追求持續** — 先建起來，再慢慢變好
