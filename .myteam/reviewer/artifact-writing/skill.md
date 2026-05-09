---
name: "artifact-writing"
description: "Write and commit reviewer artifacts with the final PASS, CONDITIONAL PASS, or FAIL outcome."
---

# Reviewer Artifact Writing

Load this skill only when `reviewer_report.md` and `reviewer_result.json` are about to be written and committed.

## Required Artifacts

- `reviewer_report.md`
  - final human-readable feature review report
- `reviewer_result.json`
  - machine-readable feature-level summary

## Shared Rules

- Write the required reviewer artifact files in the resolved shared artifact directory.
- Stage and commit only the reviewer artifact files created by the reviewer.
- Include the final outcome `PASS`, `CONDITIONAL PASS`, or `FAIL`.
