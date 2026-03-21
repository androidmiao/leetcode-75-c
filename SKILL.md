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
- maintain `README.md`, `*_Description.md`, `*_Editorial.md`, or `*_GPT-*.md`

## Folder styles

This repo has two common layouts.

### Standalone style

Usually contains:

- one `Problem Name.c`
- `*_Description.md`
- `*_Editorial.md`
- `*_GPT-*.md`

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
  - `*_GPT-*.md`

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

- `*_Description.md` has been synced from the Description page
- `*_Editorial.md` has been synced from the Editorial page when the editorial is accessible
- `*_Editorial.md` contains the full Solution section structure as repo-maintainable English notes plus corresponding Chinese translation / paraphrase
- the final report clearly states whether the Editorial content came from the live authenticated LeetCode session or from a fallback source because access was blocked

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
6. When writing notes into `*_GPT-*.md`, explicitly state:
   - the baseline approach
   - the recommended Google interview approach
   - why that recommendation is better for interview discussion

### When implementing from the Description

If the user asks you to use the problem description to implement the solution in the target `.c` file:

1. Write the final LeetCode-submit version directly into the requested `.c` file.
2. Keep the code readable even when the comments are dense.
3. If the user asks for detailed explanation, add detailed Chinese explanation comments.
4. If the user asks for line-by-line comments, annotate each major line or small block in Chinese.
5. If the user asks for a diagram, prefer an ASCII diagram in a top-of-file comment unless they explicitly want markdown instead.
6. Use a top-of-file overview comment plus local inline comments rather than turning the code into noise.

## Markdown rules

When maintaining markdown notes:

- `README.md`: short practical explanation of the chosen approach
- `*_Description.md`: problem statement notes
- `*_Editorial.md`: structured algorithm explanation and complexity notes
- `*_GPT-*.md`: working notes, comparisons, and AI-assisted reasoning for that problem

Naming rule for algorithm-explanation notes:

- for newly created notes, always use `*_Editorial.md`
- do not create new `*_Solution.md` files
- if a note is serving as the algorithm/editorial write-up for the current task, rename or maintain it as `*_Editorial.md` when the user asks
- do not rename unrelated existing `*_Solution.md` files repo-wide unless the user explicitly requests that cleanup

Update only the files relevant to the request.

### Bilingual formatting default

Unless the user explicitly asks for interleaved bilingual formatting:

- put the full English section first
- put the full Chinese translation / paraphrase after that
- do not alternate English and Chinese paragraph by paragraph

### Syncing LeetCode pages into markdown

When the user asks you to open a LeetCode problem, Description page, or Editorial page and update local markdown:

1. Prefer the existing authenticated browser session through Chrome DevTools MCP.
2. Verify whether the session is already logged in before asking the user to sign in.
3. If the target page redirects away from the requested tab, reopen it after login and confirm the final URL.
4. For `*_Description.md`, capture the problem statement, examples, constraints, and follow-up.
5. For `*_Editorial.md`, capture the official Solution section structure, approach names, core ideas, algorithm steps, and complexity notes.
6. Keep the technical structure close to the live LeetCode page.
7. Write a high-fidelity rewrite instead of pasting long official text verbatim.
8. If the file already exists, update it in place instead of creating a duplicate.
9. In the final report, say whether the content came from the live authenticated session or whether access was blocked.
10. When the request is part of new-problem initialization from a LeetCode link, do both Description and Editorial sync in the same initialization flow unless the user explicitly asks to skip one.
11. For `*_Editorial.md`, include the full set of Solution approaches that are visible on the page in English repo notes first, then add the corresponding Chinese translation / paraphrase after that.

## Comparing implementation vs. notes

When the user asks things like "is this the same as your solution?" or "which is better?":

1. Compare the current implementation against the referenced editorial file directly.
2. State whether they are the same algorithm, a variation of the same idea, or different approaches.
3. Judge "which is better" in context:
   - theoretical complexity
   - implementation difficulty in C
   - readability / teachability for this repo
   - whether the answer depends on the problem constraints
4. If a `*_GPT-*.md` file exists and the user wants the result saved, record the comparison there.

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
- Do not overwrite user-authored `*_GPT-*.md` notes unless the user asked for that file to be updated.
- Do not generate empty companion markdown files just because the template allows them.
