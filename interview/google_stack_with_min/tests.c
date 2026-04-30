/*
 * tests.c — 對 solution.c 的完整測試
 *
 * 編譯：
 *   cc -std=c11 -Wall -Wextra -fsanitize=address,undefined -g \
 *      -DSOLUTION_LIB tests.c solution.c -o tests && ./tests
 *
 * 設計重點：
 *   1. 每個 EXPECT_EQ 都比對「精確值」，不只是「非零」。
 *   2. 多個案例特別針對之前找出的 bug 設計：
 *      - 空棧 pop/peek/min → *err 應為 -1
 *      - 非空 peek → *err 應被寫入為 0（catch 漏寫 *err 的 bug）
 *      - push(-1) 後 pop → 值為 -1、err 為 0（catch 把 -1 當錯誤碼的 bug）
 *      - 推 N 個再彈 N 個 → 順序與值都正確（catch off-by-one）
 *   3. 隨機 push/pop 序列 + 參考實作對拍，補足人腦想不到的邊界。
 *   4. 每個測試前呼叫 stack_reset() 確保乾淨狀態。
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int  pop(int *err);
extern int  peek(int *err);
extern void push(int value);
extern int  min(int *err);
extern int  is_empty(void);
extern void stack_reset(void);

static int g_pass = 0;
static int g_fail = 0;
static int g_curr_failed = 0;

#define POISON 0x5A5A5A  /* 用於 *err 的「未寫入」哨兵 */

#define EXPECT_EQ(actual, expected, label) do {                          \
    long _a = (long)(actual), _e = (long)(expected);                     \
    if (_a != _e) {                                                      \
        printf("\n    FAIL: %s — expected %ld, got %ld  (%s:%d)",        \
               label, _e, _a, __FILE__, __LINE__);                       \
        g_curr_failed = 1;                                               \
    }                                                                    \
} while (0)

#define RUN(test) do {                                                   \
    stack_reset();                                                       \
    g_curr_failed = 0;                                                   \
    printf("  %-44s ", #test);                                           \
    test();                                                              \
    if (g_curr_failed) { printf("\n"); g_fail++; }                       \
    else { printf("PASS\n"); g_pass++; }                                 \
} while (0)

/* ---------------- Tests ---------------- */

static void test_initial_is_empty(void) {
    EXPECT_EQ(is_empty(), 1, "initial is_empty");
}

static void test_pop_empty_sets_err(void) {
    int err = POISON;
    pop(&err);
    EXPECT_EQ(err, -1, "pop on empty must set *err = -1");
}

static void test_peek_empty_sets_err(void) {
    int err = POISON;
    peek(&err);
    EXPECT_EQ(err, -1, "peek on empty must set *err = -1");
}

static void test_min_empty_sets_err(void) {
    int err = POISON;
    min(&err);
    EXPECT_EQ(err, -1, "min on empty must set *err = -1");
}

static void test_push_then_peek(void) {
    int err = POISON;
    push(42);
    int v = peek(&err);
    EXPECT_EQ(v, 42, "peek value");
    EXPECT_EQ(err, 0, "peek must set *err = 0 on success");  /* Bug 3 */
}

static void test_push_then_pop(void) {
    int err = POISON;
    push(42);
    int v = pop(&err);
    EXPECT_EQ(v, 42, "pop value");                           /* Bug 1 */
    EXPECT_EQ(err, 0, "pop *err on success");
    EXPECT_EQ(is_empty(), 1, "empty after pop");
}

static void test_lifo_order(void) {
    int err;
    push(10); push(20); push(30);
    EXPECT_EQ(pop(&err), 30, "pop top");                     /* Bug 1 */
    EXPECT_EQ(pop(&err), 20, "pop mid");
    EXPECT_EQ(pop(&err), 10, "pop bottom");
    EXPECT_EQ(is_empty(), 1, "empty after all pops");
}

static void test_push_minus_one_pop_returns_minus_one(void) {
    /* 原版把 -1 當錯誤碼，導致 push(-1) 後 pop 的值與「空棧錯誤」無法區分。
     * 修正版以 *err 區分，所以 -1 也是合法資料。 */
    int err = POISON;
    push(-1);
    int v = pop(&err);
    EXPECT_EQ(v, -1, "popped -1 must come back as -1");      /* Bug 4 */
    EXPECT_EQ(err, 0, "popping a real value sets *err = 0");
}

static void test_min_basic(void) {
    int err;
    push(5); push(3); push(8);
    EXPECT_EQ(min(&err), 3, "min of [5,3,8]");
    EXPECT_EQ(err, 0, "min err on success");
}

static void test_min_after_pop_top(void) {
    int err;
    push(5); push(3); push(8);
    pop(&err);  /* 移除 8 */
    EXPECT_EQ(min(&err), 3, "min after popping non-min");
    pop(&err);  /* 移除 3（當前最小）*/
    EXPECT_EQ(min(&err), 5, "min recovers after popping the min");
}

static void test_min_with_duplicates(void) {
    int err;
    push(3); push(3); push(3);
    EXPECT_EQ(min(&err), 3, "min with duplicates");
}

static void test_min_single_element(void) {
    int err;
    push(7);
    EXPECT_EQ(min(&err), 7, "min of single element");
}

static void test_resize_beyond_initial(void) {
    /* 推 100 個（初始容量 10）強制多次 realloc，再彈出全部驗證 LIFO */
    int err;
    for (int i = 0; i < 100; i++) push(i);
    for (int i = 99; i >= 0; i--) {
        EXPECT_EQ(pop(&err), i, "pop after resize");
    }
    EXPECT_EQ(is_empty(), 1, "empty after popping 100");
}

static void test_alternating_push_pop(void) {
    int err;
    for (int i = 0; i < 50; i++) {
        push(i);
        EXPECT_EQ(pop(&err), i, "pop matches just-pushed");
        EXPECT_EQ(is_empty(), 1, "empty between cycles");
    }
}

static void test_peek_does_not_pop(void) {
    int err;
    push(7);
    peek(&err);
    EXPECT_EQ(is_empty(), 0, "peek must not remove");
    EXPECT_EQ(peek(&err), 7, "peek twice returns same value");
    EXPECT_EQ(peek(&err), 7, "peek thrice returns same value");
}

static void test_random_property(void) {
    /* 對拍：把堆疊操作隨機產生，與一個簡單 reference (陣列+top) 比較。 */
    int ref[2000];
    int ref_top = 0;
    int err;
    srand(42);
    for (int op = 0; op < 5000; op++) {
        int empty = (ref_top == 0);
        /* 空時必須 push；否則隨機 push/pop/peek/min */
        int choice = empty ? 0 : (rand() % 4);
        if (choice == 0) {
            int v = (rand() % 2000) - 1000;
            push(v);
            ref[ref_top++] = v;
        } else if (choice == 1) {
            int v = pop(&err);
            EXPECT_EQ(err, 0, "rand pop err");
            EXPECT_EQ(v, ref[--ref_top], "rand pop value");
        } else if (choice == 2) {
            int v = peek(&err);
            EXPECT_EQ(err, 0, "rand peek err");
            EXPECT_EQ(v, ref[ref_top - 1], "rand peek value");
        } else {
            int v = min(&err);
            EXPECT_EQ(err, 0, "rand min err");
            int m = ref[0];
            for (int i = 1; i < ref_top; i++) if (ref[i] < m) m = ref[i];
            EXPECT_EQ(v, m, "rand min value");
        }
        if (g_curr_failed) return;  /* 第一個錯誤就停，避免 spam */
    }
}

int main(void) {
    printf("Running tests...\n\n");
    RUN(test_initial_is_empty);
    RUN(test_pop_empty_sets_err);
    RUN(test_peek_empty_sets_err);
    RUN(test_min_empty_sets_err);
    RUN(test_push_then_peek);
    RUN(test_push_then_pop);
    RUN(test_lifo_order);
    RUN(test_push_minus_one_pop_returns_minus_one);
    RUN(test_min_basic);
    RUN(test_min_after_pop_top);
    RUN(test_min_with_duplicates);
    RUN(test_min_single_element);
    RUN(test_resize_beyond_initial);
    RUN(test_alternating_push_pop);
    RUN(test_peek_does_not_pop);
    RUN(test_random_property);
    printf("\n%d passed, %d failed\n", g_pass, g_fail);
    return g_fail > 0 ? 1 : 0;
}
