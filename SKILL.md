---
name: leetcode-75-c-workflow
description: Use this skill when working in the leetcode-75-c repository to add, refactor, explain, verify, or document a LeetCode C solution while preserving the repo's standalone-first folder style and markdown note conventions.
---

# LeetCode 75 C Workflow

Use this skill for tasks inside the `leetcode-75-c` repository when the user wants to:

- solve or revise a LeetCode problem in C
- initialize a new problem folder
- explain or annotate a standalone `.c` solution
- prepare a problem with a Google LeetCode interview focus
- sync LeetCode Description / Editorial content into local markdown notes
- compare the current implementation against editorial notes
- maintain `README.md`, `*_Description.md`, `*_Editorial.md`, or `*_Sonnet-4.6.md`

## Folder styles

This repo has two common layouts.

### Standalone style

Usually contains:

- one `Problem Name.c`
- `*_Description.md`
- `*_Editorial.md`
- `*_Sonnet-4.6.md`

This is the default and preferred style for new problems.

### Newer local-test style

Usually contains:

- `solution.c`
- `solution.h`
- `main.c`
- `Makefile`
- `README.md`
- sometimes extra markdown files

Only use this layout when the folder already uses it or the user explicitly asks for it.

## New problem initialization

For a new problem, default to standalone style unless the user explicitly asks for split local-test files.

### Default new-problem shape

- `Problem Name.c`
- optional companions when requested or clearly implied by the task:
  - `*_Description.md`
  - `*_Editorial.md`
  - `*_Sonnet-4.6.md`

Use the nearest existing naming pattern in the repo. Do not normalize the entire repository.

### Standalone defaults

- put the full LeetCode-submit-ready implementation in `Problem Name.c`
- keep helper functions in the same file unless the user asks for a split-file layout
- treat the standalone `.c` file as the main teaching artifact for explanation-heavy tasks
- keep verification lightweight unless the user explicitly asks for a harness

If the user wants LeetCode page syncing during new-problem setup, also create:

- `*_Description.md`
- `*_Editorial.md`

If the user provides a LeetCode problem link during initialization, treat page syncing as part of initialization by default. In that case, initialization is not complete until:

- `*_Description.md` has been synced from the Description page, with the full English section first followed by a full Traditional Chinese (繁體中文) translation / paraphrase (same bilingual formatting as `*_Editorial.md`)
- `*_Editorial.md` has been synced from the Editorial page when the editorial is accessible
- `*_Editorial.md` contains the full Solution section structure as repo-maintainable English notes plus corresponding Traditional Chinese (繁體中文) translation / paraphrase
- `*_Sonnet-4.6.md` has been created with an implementation vs. editorial comparison (see below)
- the `.c` solution file contains detailed Traditional Chinese (繁體中文) inline comments and ASCII diagram(s) in the top-of-file block comment (see "Traditional Chinese (繁體中文) annotation and diagram rules" below)
- the final report clearly states whether the Editorial content came from the live authenticated LeetCode session or from a fallback source because access was blocked

### Creating `*_Sonnet-4.6.md` during initialization

After both the `.c` solution and `*_Editorial.md` are in place, always create `*_Sonnet-4.6.md` as part of initialization. It must contain:

1. **Implementation vs. Editorial comparison** — state whether the `.c` solution is the same algorithm as one of the editorial approaches, a variation, or a different approach entirely.
2. **Which approach is better** — compare along: time complexity, space complexity, interview suitability, and C implementation difficulty. Use a markdown table when helpful.
3. **Baseline vs. recommended** — name the baseline approach and the recommended approach, and explain why the recommended one is better for interview discussion.

Write the full English section first, then the full Traditional Chinese (繁體中文) translation / paraphrase after it.

## Working method

1. Identify the target folder and classify it as standalone, newer local-test, or new.
2. For new problems, initialize with standalone style by default.
3. Preserve local naming and surrounding conventions.
4. For standalone folders, edit the standalone `.c` directly and keep matching markdown aligned with it.
5. For newer local-test folders, keep logic in `solution.c`, declarations in `solution.h`, and lightweight checks in `main.c`.
6. If a `Makefile` exists, run the local verification flow when logic changes.

## C implementation rules

- Match LeetCode function signatures exactly.
- Prefer straightforward, interview-appropriate C.
- Use linear-time or constant-extra-space approaches when the problem calls for it.
- Avoid unnecessary dependencies or repo-wide tooling.

### Google interview mode

When the user explicitly says the goal is "for Google LeetCode interview" or equivalent:

1. Start from the simplest correct baseline and state it briefly.
2. Prefer implementing the follow-up-friendly approach when it is realistic in standalone C.
3. For top-k style questions, prefer the standard interview pattern such as size-`k` heap over a repo-only teaching simplification when the heap approach materially improves complexity.
4. Keep the final code explainable in an interview:
   - clear data-structure choice
   - explicit comparator / ordering logic
   - complexity tradeoff that can be defended aloud
5. If the theoretically best editorial approach is too implementation-heavy for a strong interview answer in standalone C, prefer the best balanced approach rather than the most exotic one.
6. When writing notes into `*_Sonnet-4.6.md`, explicitly state:
   - the baseline approach
   - the recommended Google interview approach
   - why that recommendation is better for interview discussion

### Traditional Chinese (繁體中文) annotation and diagram rules

Every `.c` solution produced or updated in this repo must include:

1. **Top-of-file block comment** containing:
   - problem number, title, difficulty, time/space complexity
   - a step-by-step algorithm walkthrough with at least one concrete example
   - ASCII diagram(s) showing the key pointer manipulation or data-structure state changes at each step
   - for linked-list problems: diagrams for the pointer-finding phase (e.g. fast/slow), the reversal phase, the comparison phase, and the restoration phase
   - for tree/graph problems: ASCII tree or adjacency sketches showing before/after state
2. **Traditional Chinese (繁體中文) inline comments** on every non-trivial line or logical block:
   - what the variable represents
   - why the condition is written the way it is
   - what invariant holds at that point
3. Prefer `/*  */` block comments for multi-line diagrams; use `/* … */` or `//` for inline notes.
4. Keep the code itself unchanged; add comments only around existing logic.
5. Use the top-of-file overview block first, then keep inline comments concise (1–2 lines each).

This annotation step is **part of initialization** when a LeetCode link is provided. It must be applied to the `.c` file before initialization is considered complete.

### When implementing from the Description

If the user asks you to use the problem description to implement the solution in the target `.c` file:

1. Write the final LeetCode-submit version directly into the requested `.c` file.
2. Apply the Traditional Chinese (繁體中文) annotation and diagram rules above as part of the same step.
3. Keep the code readable even when the comments are dense.
4. Use a top-of-file overview comment plus local inline comments rather than turning the code into noise.

## Markdown rules

When maintaining markdown notes:

- `README.md`: short practical explanation of the chosen approach
- `*_Description.md`: problem statement notes
- `*_Editorial.md`: structured algorithm explanation and complexity notes
- `*_Sonnet-4.6.md`: working notes, comparisons, and AI-assisted reasoning for that problem

Naming rule for algorithm-explanation notes:

- for newly created notes, always use `*_Editorial.md`
- do not create new `*_Solution.md` files
- if a note is serving as the algorithm/editorial write-up for the current task, rename or maintain it as `*_Editorial.md` when the user asks
- do not rename unrelated existing `*_Solution.md` files repo-wide unless the user explicitly requests that cleanup

Update only the files relevant to the request.

### Bilingual formatting default

Unless the user explicitly asks for interleaved bilingual formatting:

- put the full English section first
- put the full Traditional Chinese (繁體中文) translation / paraphrase after that
- do not alternate English and Traditional Chinese (繁體中文) paragraph by paragraph

### Syncing LeetCode pages into markdown

When the user asks you to open a LeetCode problem, Description page, or Editorial page and update local markdown:

1. Prefer the existing authenticated browser session through Chrome DevTools MCP.
2. Verify whether the session is already logged in before asking the user to sign in.
3. If the target page redirects away from the requested tab, reopen it after login and confirm the final URL.
4. For `*_Description.md`, capture the problem statement, examples, constraints, and follow-up. Then append a full Traditional Chinese (繁體中文) translation / paraphrase after the English section (same bilingual formatting default as `*_Editorial.md`).
5. For `*_Editorial.md`, capture the official Solution section structure, approach names, core ideas, algorithm steps, and complexity notes.
6. Keep the technical structure close to the live LeetCode page.
7. Write a high-fidelity rewrite instead of pasting long official text verbatim.
8. If the file already exists, update it in place instead of creating a duplicate.
9. In the final report, say whether the content came from the live authenticated session or whether access was blocked.
10. When the request is part of new-problem initialization from a LeetCode link, do both Description and Editorial sync in the same initialization flow unless the user explicitly asks to skip one.
11. For `*_Editorial.md`, include the full set of Solution approaches that are visible on the page in English repo notes first, then add the corresponding Traditional Chinese (繁體中文) translation / paraphrase after that.

## Linux Kernel connection notes (`*_Linux.md`)

When the user mentions a Linux kernel connection (e.g. "kernel 的 list_sort 就是 merge sort 變體") or asks about how a problem relates to real-world kernel code:

1. Create a `*_Linux.md` file in the problem folder (e.g. `23_Merge k Sorted Lists_Linux.md`).
2. The file must contain:
   - **Linux 相關原始碼位置** — list the relevant kernel source files (`lib/`, `include/linux/`, test files, etc.) with a brief description of each file's role.
   - **Kernel vs. LeetCode 實作對照** — compare the kernel implementation against the repo's `.c` solution along dimensions such as: algorithm direction (top-down vs. bottom-up), data structure, merge strategy, stability, complexity, API design (hardcoded vs. generic comparator).
   - **主管 Code Review 角度考題** — enumerate questions a senior engineer or manager familiar with the kernel codebase might ask during code review, including: expected answer direction, follow-up probes, and what each question is really testing (cache locality awareness, pointer mastery, defensive programming, stack budget, etc.).
   - **面試加分總結** — bullet points on how to leverage kernel knowledge to stand out in an interview.
3. Write in Traditional Chinese (繁體中文) by default (matching the repo's annotation convention), with kernel identifiers and code snippets kept in English.
4. Include a **Sources** section at the end with links to the relevant kernel source on GitHub, LWN articles, or LKML threads.

This file is created on demand (when the user raises the kernel connection), not as part of default initialization.

## Comparing implementation vs. notes

When the user asks things like "is this the same as your solution?" or "which is better?":

1. Compare the current implementation against the referenced editorial file directly.
2. State whether they are the same algorithm, a variation of the same idea, or different approaches.
3. Judge "which is better" in context:
   - theoretical complexity
   - implementation difficulty in C
   - readability / teachability for this repo
   - whether the answer depends on the problem constraints
4. If a `*_Sonnet-4.6.md` file exists and the user wants the result saved, record the comparison there.

## Recommended commands

Prefer fast local inspection:

```bash
rg --files .
rg -n "pattern" <folder>
sed -n '1,200p' <file>
make
```

For a newer local-test folder with a `Makefile`, use `make` to verify changes.

## Avoid

- Do not rewrite standalone folders into the newer layout unless explicitly requested.
- Do not normalize the repo's naming inconsistencies.
- Do not add a global build system.
- Do not create broad repo-wide refactors for a single problem.
- Do not overwrite user-authored `*_Sonnet-4.6.md` notes unless the user asked for that file to be updated.
- Do not generate empty companion markdown files just because the template allows them.
