# 20. Valid Parentheses

## Problem
Determine if input string of brackets is valid. Every open bracket must have a corresponding close bracket.

## Key Insight
**Stack**: Push open brackets, pop and match for close brackets.

## Examples
- "()" → true
- "()[]{}" → true
- "(]" → false
- "([)]" → false
