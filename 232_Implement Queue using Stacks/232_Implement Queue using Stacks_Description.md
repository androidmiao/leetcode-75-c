# 232. Implement Queue using Stacks

## Problem
Implement a queue using two stacks.

## Key Insight
**Two Stacks with Lazy Transfer**: Push to inbox, pop from outbox.
Transfer only when outbox is empty.

## Operations
- push(x): O(1)
- pop(): Amortized O(1)
- peek(): Amortized O(1)
- empty(): O(1)
