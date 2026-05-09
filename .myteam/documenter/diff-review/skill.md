---
name: "diff-review"
description: "Compare the actual implemented and tested diff against plan hints to determine documentation impact."
---

# Documenter Diff Review

Load this skill only when the documenter must determine what actually changed and what documentation impact follows from that diff.

## Required Actions

- Read the diff between the Tester branch and the relevant base branch or commit.
- Inspect both implementation and test changes to understand final shipped behavior.
- Use the plan's `Documentation Impact` section only as a discovery aid, not as the source of truth.
- Identify behavior already documented, documentation gaps, and any now-outdated statements.
- If the exact base branch or commit is not explicit, determine the most likely comparison base from repository and branch context and label the choice as an assumption.

## Limits

- Do not document planned behavior that is absent from the actual diff.
- Do not let the plan override the implemented and tested behavior.
