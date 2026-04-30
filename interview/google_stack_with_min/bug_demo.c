/*
 * bug_demo.c — 對 original.c 的 bug 個別示範
 *
 * 編譯：
 *   gcc -std=c11 -Wall -Wextra -fsanitize=address,undefined -g \
 *       bug_demo.c original.c -o bug_demo
 *
 * 執行（每個 demo 一次）：
 *   ./bug_demo pop      # off-by-one in pop
 *   ./bug_demo peek     # off-by-one in peek
 *   ./bug_demo min      # uninit return on empty
 *   ./bug_demo err      # peek doesn't write *err on success
 *   ./bug_demo api      # API design bug: -1 ambiguous
 *
 * 關於 sanitizer 抓不到的 bug：
 *   off-by-one 讀的位置 values_[count] 在 realloc 配置的 10 格範圍內，
 *   ASan 不會 fire（它只抓 OOB，不抓未初始化讀取）。在這個沙盒沒有
 *   MSan / Valgrind 的情況下，我們改用「投毒記憶體 + 比對哨兵值」
 *   的手法，讓 bug 的回傳值清楚可見。
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* original.c 的全域變數（非 static，可從外部看見） */
extern int* values_;
extern int  values_size_;
extern int  values_count_;

/* original.c 的 API */
extern int  pop(int *err);
extern int  peek(int *err);
extern void push(int value);
extern int  min(void);          /* 注意：原版無 *err 參數 */
extern int  is_empty(void);

#define POISON_VAL  0xCAFEBABE  /* 用於陣列槽位 */
#define POISON_ERR  0x5A5A5A    /* 用於 *err */

static void print_section(const char *name) {
    printf("\n========== %s ==========\n", name);
}

/* ------------------------------------------------------------ */
/* Bug 1: pop 的差一錯誤
 *
 * push(2) 後 values_count_ = 1。原版的
 *     return values_[values_count_--];
 * 後置遞減，先讀 values_[1]（陣列裡尚未寫入的位置），再讓 count 變 0。
 *
 * 我們把 values_[1] 投毒成 0xCAFEBABE，這樣若 bug 存在，回傳值就是 CAFEBABE
 * 而不是 push 進去的 2。
 */
static void demo_pop_oob(void) {
    print_section("Bug 1: pop off-by-one");

    push(2);
    values_[1] = (int)POISON_VAL;     /* 投毒 idx 1 */

    int err = 0;
    int v = pop(&err);
    printf("expected: 2 (err=0)\n");
    printf("actual:   %d (err=%d)  [hex: 0x%X]\n", v, err, (unsigned)v);
    if ((unsigned)v == POISON_VAL) {
        printf(">>> BUG CONFIRMED: pop read uninitialized slot values_[count]\n");
    } else {
        printf("    (no obvious bug — possibly already fixed)\n");
    }
}

/* ------------------------------------------------------------ */
/* Bug 2: peek 的差一錯誤（同樣手法）*/
static void demo_peek_oob(void) {
    print_section("Bug 2: peek off-by-one");

    push(2);
    values_[1] = (int)POISON_VAL;

    int err = 0;
    int v = peek(&err);
    printf("expected: 2 (err=0)\n");
    printf("actual:   %d (err=%d)  [hex: 0x%X]\n", v, err, (unsigned)v);
    if ((unsigned)v == POISON_VAL) {
        printf(">>> BUG CONFIRMED: peek read uninitialized slot values_[count]\n");
    }
}

/* ------------------------------------------------------------ */
/* Bug 3: peek 在成功路徑沒寫 *err
 *
 * 故意把 err 投毒成 POISON_ERR，呼叫 peek 後若 err 沒被改寫，
 * 就證明 *err = 0 漏了。
 */
static void demo_peek_err_not_written(void) {
    print_section("Bug 3: peek doesn't write *err on success");

    push(2);
    int err = POISON_ERR;
    int v = peek(&err);
    printf("expected: peek returns 2, *err = 0\n");
    printf("actual:   v=%d, err=0x%X\n", v, err);
    if (err == POISON_ERR) {
        printf(">>> BUG CONFIRMED: peek left *err untouched on success\n");
    }
}

/* ------------------------------------------------------------ */
/* Bug 5: min 在空棧回傳未初始化變數
 *
 * 編譯時 -Wuninitialized 已經會警告。執行時若加 -O2 或某些優化等級，
 * 編譯器可能把 v 放在 register 裡，內容是垃圾——最終回傳值不可預測。
 *
 * 我們呼叫多次比對結果，理論上一個正確的函式應該每次回傳同樣的東西。
 */
static void demo_min_uninit_on_empty(void) {
    print_section("Bug 5: min returns uninitialized on empty stack");

    int v1 = min();
    int v2 = min();
    int v3 = min();
    printf("min() called 3 times on empty stack:\n");
    printf("  call 1: %d\n", v1);
    printf("  call 2: %d\n", v2);
    printf("  call 3: %d\n", v3);
    printf("(values may differ between runs/builds — UB territory)\n");
    printf(">>> BUG CONFIRMED at compile time: see -Wuninitialized warning\n");
}

/* ------------------------------------------------------------ */
/* Bug 4: API 設計——-1 同時是錯誤碼和合法資料
 *
 * 原版 pop 在空棧時 *err = -1 且 return *err（也就是 -1）。
 * 假設我們先 fix 了 off-by-one，但 pop 仍然回傳 -1 當錯誤碼，
 * 那麼呼叫者拿到 -1 時無法判斷是「彈出值就是 -1」還是「空棧錯誤」。
 *
 * 我們用 push(-1) 後 pop 與 直接 pop 兩種情境對比：
 *   情境 A: 空棧 pop → 回傳值 = -1, err = -1
 *   情境 B: push(-1) 後 pop → 回傳值 = ?  (原版因 bug 1 是垃圾)
 *
 * 即使 bug 1 已修，情境 A 與 B 的「回傳值」都會是 -1，呼叫者僅靠回傳值
 * 無法區分——必須看 *err。這正說明為何「錯誤碼塞回傳值」是糟糕設計。
 */
static void demo_api_design_minus_one(void) {
    print_section("Bug 4: -1 is ambiguous (error code vs legit value)");

    /* 情境 A: 空棧 pop */
    int errA;
    int vA = pop(&errA);
    printf("Scenario A: pop on empty stack\n");
    printf("  return = %d, err = %d   <-- err=-1 means error\n", vA, errA);

    /* 情境 B: push(-1) 後 pop */
    push(-1);
    /* 預先把 idx 1 投毒，凸顯原版 off-by-one */
    if (values_size_ > 1) values_[1] = (int)POISON_VAL;
    int errB;
    int vB = pop(&errB);
    printf("\nScenario B: push(-1) then pop\n");
    printf("  return = %d, err = %d   <-- err=0 means success\n", vB, errB);

    printf("\nIf caller only inspects the return value, they cannot tell\n");
    printf("scenario A from a (correctly fixed) scenario B — both can\n");
    printf("legitimately return -1. Caller MUST check *err.\n");
    printf(">>> Lesson: don't conflate error code with payload.\n");
}

/* ------------------------------------------------------------ */
int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr,
            "Usage: %s <demo>\n"
            "  pop   - Bug 1: off-by-one in pop\n"
            "  peek  - Bug 2: off-by-one in peek\n"
            "  err   - Bug 3: peek doesn't write *err on success\n"
            "  api   - Bug 4: -1 is ambiguous (API design)\n"
            "  min   - Bug 5: min on empty stack returns uninitialized\n"
            "  all   - run all demos sequentially\n",
            argv[0]);
        return 2;
    }

    const char *which = argv[1];
    if (!strcmp(which, "pop"))      demo_pop_oob();
    else if (!strcmp(which, "peek")) demo_peek_oob();
    else if (!strcmp(which, "err"))  demo_peek_err_not_written();
    else if (!strcmp(which, "api"))  demo_api_design_minus_one();
    else if (!strcmp(which, "min"))  demo_min_uninit_on_empty();
    else if (!strcmp(which, "all")) {
        demo_pop_oob();
        /* 注意：每個 demo 之間共用全域狀態，但因為 main 結束程式就會重置 */
        /* 為了 "all" 模式仍乾淨，這裡簡單呼叫 free + reset。            */
        free(values_); values_ = NULL; values_size_ = 0; values_count_ = 0;
        demo_peek_oob();
        free(values_); values_ = NULL; values_size_ = 0; values_count_ = 0;
        demo_peek_err_not_written();
        free(values_); values_ = NULL; values_size_ = 0; values_count_ = 0;
        demo_api_design_minus_one();
        free(values_); values_ = NULL; values_size_ = 0; values_count_ = 0;
        demo_min_uninit_on_empty();
    }
    else {
        fprintf(stderr, "unknown demo: %s\n", which);
        return 2;
    }
    return 0;
}
