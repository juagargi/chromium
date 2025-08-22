# AGENTS.md — Guidance for Coding Agents (OpenAI Codex, etc.)

## Project overview
This repository is a **GitHub mirror** of the Chromium codebase, used for **local code navigation, patch authoring, static analysis, and small, self-contained unit tests**. It is **not intended to build full Chromium** in this environment.

If you (agent) are proposing changes, **limit scope** to clearly defined components and produce minimal diffs with strong reasoning and references to call sites and tests.

## What you MUST do
- Work **incrementally** and explain your plan before large edits.
- Keep changes **small and localized** (one component or directory at a time).
- Prefer **mechanical, safe refactors** (API tightenings, nullptr checks, dead-code removal, comments, docs, small perf fixes).
- Run **formatters/linters** on changed files.
- Provide:
  - A **one-paragraph summary** of the change,
  - A **bullet list of files touched** and why,
  - **Risk & rollback plan**,
  - **Suggested reviewers** (based on OWNERS/README where present).

## What you MUST NOT do
- Do **NOT** attempt a full Chromium build or a full `gclient sync`.
- Do **NOT** fetch large third-party dependencies or toolchains.
- Do **NOT** modify unrelated directories in the same PR.

## Workspace assumptions
This workspace may use **sparse checkout**. Only a subset of paths may exist locally.
If you need more files, ask to expand sparse paths (example shown below).

### Sparse checkout helper (human or agent can run)
```bash
# Add paths relevant to the task; keep the set minimal:
git sparse-checkout set \
  base/ build/ tools/ buildtools/ \
  third_party/abseil-cpp/
# Examples for common areas (uncomment as needed):
# net/ url/
# content/ third_party/blink/
# crypto/
```
