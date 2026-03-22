/*
 * LeetCode 20. 有效的括號
 * 難度：Easy
 * 演算法：棧，推入開括號，彈出並匹配閉括號
 * 時間：O(n)，空間：O(n)
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

bool isValid(char* s) {
    int len = strlen(s);
    if (len == 0) return true;
    if (len % 2 == 1) return false;

    char* stack = (char*)malloc(len * sizeof(char));
    int top = -1;

    for (int i = 0; i < len; i++) {
        char c = s[i];

        if (c == '(' || c == '[' || c == '{') {
            stack[++top] = c;
        } else {
            if (top == -1) {
                free(stack);
                return false;
            }

            char last = stack[top--];

            if ((c == ')' && last != '(') ||
                (c == ']' && last != '[') ||
                (c == '}' && last != '{')) {
                free(stack);
                return false;
            }
        }
    }

    bool result = (top == -1);
    free(stack);
    return result;
}
