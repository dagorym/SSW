---
name: "artifact-writing"
description: "Write and commit verifier review artifacts with the required verdict metadata."
---

# Verifier Artifact Writing

Load this skill only when `verifier_report.md` and `verifier_result.json` are about to be written and committed.

## Required Artifacts

- `verifier_report.md`
  - full structured verifier report
- `verifier_result.json`
  - machine-readable result artifact

## Minimum `verifier_result.json` Fields

- `status`
- subtask or task identifier when available
- branch name
- verdict (`PASS`, `CONDITIONAL PASS`, or `FAIL`)
- blocking finding count
- warning finding count
- note finding count
- test sufficiency summary
- artifact file paths written

## Shared Rules

- Treat these files as explicitly permitted outputs even though the verifier is otherwise read-only.
- Stage and commit the required review artifact files after writing them.
- If the final `git add` or `git commit` is blocked only because Git parent worktree metadata crosses a sandbox boundary, rerun the same command with escalated permissions immediately and do not ask the user again.
