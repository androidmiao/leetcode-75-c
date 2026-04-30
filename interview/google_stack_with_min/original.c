/*
 * Google Interview — Stack with Min
 * 原始版本（含多處 bug，僅供對照用）
 *
 * 面試情境：請實作一個整數堆疊，支援以下操作：
 *   push(value)     將值推入棧頂
 *   pop(&err)       彈出棧頂並回傳；空棧時透過 *err 回報
 *   peek(&err)      查看棧頂；空棧時透過 *err 回報
 *   is_empty()      回傳棧是否為空
 *   min()           回傳棧中最小值
 *
 * 候選人交出的版本如下，請找出所有問題並修正：
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int* values_ = NULL;
int values_size_ = 0;
int values_count_ = 0;

void make_size() {
  if (values_size_ <= values_count_) {
    values_size_ += 10;
    values_ = realloc(values_, values_size_ * sizeof(int));
    assert(values_ != NULL);
  }
}

int is_empty() {
  if (values_count_ == 0) {
    return 1;
  }
  return 0;
}

int pop(int *err) {
  if (is_empty()) {
    *err = -1;
    return *err;
  }
  *err = 0;
  return values_[values_count_--];   /* BUG: 後置遞減，讀到 count 而非 count-1 */
}

int peek(int *err) {
  if (is_empty()) {
    *err = -1;
  }
  return values_[values_count_];     /* BUG: 索引應為 count-1；成功時未設 *err */
}

void push(int value) {
  make_size();
  values_[values_count_++] = value;
}

int min() {                          /* BUG: 空棧時 v 未初始化即被回傳 */
  int first = 1;
  int v;
  for (int i = 0; i < values_count_; i++) {
    if (first) {
      v = values_[i];
      first = 0;
    } else if (values_[i] < v) {
      v = values_[i];
    }
  }
  return v;
}
