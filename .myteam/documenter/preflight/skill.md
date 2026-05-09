---
name: "preflight"
description: "Restate documentation scope, recover plan and artifact context, and begin in the same run."
---

# Documenter Preflight

Load this skill immediately after the documenter has confirmed that blocking inputs are present and documentation work should continue in the same run.

## Required Actions

- Confirm the current worktree is branched from the Tester branch.
- Restate the documentation scope in terms of the implemented story, not the epic summary.
- Identify the story-specific plan document when provided or recoverable from repository context.
- Reuse a provided shared artifact directory as a repository-root-relative path.
- If no shared artifact path is provided, derive repository-root-relative `artifacts/<task-slug>` from the task name.
- When deriving `<task-slug>`, remove trailing agent-role suffixes such as `implementer`, `tester`, `documenter`, and `verifier`.
- Label inferred plan-path, artifact-path, or comparison-base decisions as assumptions.

## Startup Contract

- Do not treat the scope restatement as task completion.
- After the restatement, immediately inspect the relevant documentation files or implemented diff in the same run.

## Limits

- Do not infer around missing documentation scope when repository evidence is insufficient.
