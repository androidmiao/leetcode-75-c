/*
 * Google Interview — Stack with Min
 * 修正版
 *
 * 主要修正：
 *   1. pop 改用前置遞減 --values_count_，讀到正確的棧頂並只少 1。
 *   2. peek 改讀 values_[values_count_ - 1]，並補上成功路徑的 *err = 0。
 *   3. pop / peek / min 不再用 -1 當回傳值代表錯誤；錯誤一律走 *err。
 *      這樣 -1 也能合法地當作棧內資料。
 *   4. min 在空棧時透過 *err 回報，避免回傳未初始化變數。
 *   5. is_empty 改為單行運算式。
 *   6. 全域變數加上 static，限制連結範圍。
 *
 * 時間複雜度：push / pop / peek / is_empty 均攤 O(1)；min O(n)。
 * 空間複雜度：O(n)。
 *
 * 進階追問（見 README）：min 能否做到 O(1)？答案：可以，用平行的「最小值棧」。
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static int* values_      = NULL;
static int  values_size_ = 0;
static int  values_count_ = 0;

/* 確保至少還有一格可寫入，必要時擴容 */
static void make_size(void) {
    if (values_size_ <= values_count_) {
        values_size_ += 10;
        values_ = realloc(values_, values_size_ * sizeof(int));
        assert(values_ != NULL);
    }
}

int is_empty(void) {
    return values_count_ == 0;
}

/* 成功時 *err = 0 並回傳彈出的值；
 * 失敗（空棧）時 *err = -1，回傳值未定義（呼叫者必須先檢查 err）。 */
int pop(int* err) {
    if (is_empty()) {
        *err = -1;
        return 0;
    }
    *err = 0;
    return values_[--values_count_];   /* 前置遞減：讀取 count-1 並更新 count */
}

/* 同樣以 *err 區分成功 / 失敗 */
int peek(int* err) {
    if (is_empty()) {
        *err = -1;
        return 0;
    }
    *err = 0;
    return values_[values_count_ - 1]; /* 棧頂在 count-1 */
}

void push(int value) {
    make_size();
    values_[values_count_++] = value;
}

/* 線性掃描求最小值；空棧時回報錯誤 */
int min(int* err) {
    if (is_empty()) {
        *err = -1;
        return 0;
    }
    *err = 0;
    int v = values_[0];
    for (int i = 1; i < values_count_; i++) {
        if (values_[i] < v) v = values_[i];
    }
    return v;
}

/* 測試輔助：清空堆疊狀態（讓每個測試都從乾淨狀態開始） */
void stack_reset(void) {
    free(values_);
    values_ = NULL;
    values_size_ = 0;
    values_count_ = 0;
}

/* ------------------------------------------------------------ */
/* 簡易 driver：示範題目要求的操作序列                              */
/*   push(2), peek, push(8), peek, push(1), pop, min               */
/* ------------------------------------------------------------ */
/* 用 -DSOLUTION_LIB 編譯時隱藏 main，讓此檔可作為 library 連結進測試 */
#ifndef SOLUTION_LIB
int main(void) {
    int err;

    push(2);
    printf("push(2)\n");

    printf("peek -> %d (err=%d)\n", peek(&err), err);

    push(8);
    printf("push(8)\n");

    printf("peek -> %d (err=%d)\n", peek(&err), err);

    push(1);
    printf("push(1)\n");

    printf("pop  -> %d (err=%d)\n", pop(&err), err);
    printf("min  -> %d (err=%d)\n", min(&err), err);

    /* 預期輸出：
     *   push(2)
     *   peek -> 2 (err=0)
     *   push(8)
     *   peek -> 8 (err=0)
     *   push(1)
     *   pop  -> 1 (err=0)
     *   min  -> 2 (err=0)
     */
    return 0;
}
#endif  /* SOLUTION_LIB */
