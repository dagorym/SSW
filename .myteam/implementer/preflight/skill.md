---
name: "preflight"
description: "Restate implementation scope, select validation guidance, and begin work in the same run."
---

# Implementer Preflight

Load this skill immediately after the implementer has confirmed that blocking inputs are present and work should continue in the same run.

## Purpose

Use this skill to turn the approved task into a concrete implementation starting point before substantive edits begin.

## Required Actions

- Read the entire approved plan or subtask before editing.
- Extract and restate:
  - task goal
  - allowed files
  - acceptance criteria
  - validation commands when known
- Identify test file location guidance for the Tester.
- If validation commands are not provided, determine the smallest relevant existing validation from repository conventions and label the choice as an assumption.
- If Tester test-location guidance is not provided, infer the most likely location from repository conventions and label the choice as an assumption.
- Treat required shared artifact directory files as allowed outputs even when they fall outside the plan's allowed file list.
- Refuse scope expansion beyond approved files or criteria.

## Startup Contract

- Do not treat the preflight restatement as task completion.
- After the restatement, immediately begin the first safe in-scope repository inspection, validation selection, or implementation step in the same run.
- The first substantive response must include the preflight restatement, the next concrete action, and evidence that the action has begun.

## Limits

- Do not infer around missing allowed files or missing acceptance criteria.
- Do not begin editing until the approved task boundaries are clear enough to proceed safely.
