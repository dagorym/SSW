---
name: "tester-handoff"
description: "Compose the success-path Tester handoff for stdout and tester_prompt.txt."
---

# Implementer Tester Handoff

Load this skill only when the implementation has reached the success path and the downstream Tester prompt is about to be written or reported.

## Purpose

Use this skill to produce a ready-to-run Tester handoff that allows testing to begin immediately without another clarification turn.

## Required Content

The Tester handoff must include:

- modified files
- acceptance criteria from the Planner
- exact test file location guidance when known, otherwise the most likely location labeled as an assumption
- implementation context including behavior changes, relevant entry points, flags or config, and important edge cases
- exact validation or test commands when known, otherwise the smallest relevant existing test command(s) labeled as assumptions
- any existing validations that fail only because approved behavior intentionally changed
- the shared repository-root-relative artifact directory path to reuse
- an instruction to infer missing test-location or test-command details from repository conventions when safe
- an instruction to continue in the same run when blockers are absent
- the explicit completion gate:
  - `Do not report success unless all required artifacts exist and all changes are committed.`

## Stdout And File Contract

- In stdout, present the handoff as a `Tester Agent Prompt` block.
- In `tester_prompt.txt`, omit the heading line `Tester Agent Prompt` and write only the handoff body.
- Start the handoff body with the exact line `Your role is 'tester'. Your task is as follows:`

## Template

```text
Tester Agent Prompt
Your role is 'tester'. Your task is as follows:

Task summary:
- <short implementation summary>

Modified files:
- <file path 1>
- <file path 2>

Acceptance criteria to validate (from Planner):
- <AC 1>
- <AC 2>

Create test files in:
- <exact directory or file path pattern, or the most likely location labeled as an assumption>

Implementation context for testing:
- <behavior changes and impacted components>
- <configuration/flags/env vars relevant for tests>
- <edge cases or regressions to target>

Suggested test command(s):
- <exact command when known, or the smallest relevant existing command labeled as an assumption>

Startup behavior:
- If acceptance criteria and implementation context are present, begin testing work immediately and continue in the same run.
- Infer missing test-location or test-command details from repository conventions when repository evidence is sufficient, and label those choices as assumptions instead of treating them as blockers.
- Do not stop after activation, directory discovery, artifact-directory confirmation, or framework discovery when testing can proceed.

Shared artifact directory:
- <repository-root-relative shared artifact directory path to reuse>

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.
```
