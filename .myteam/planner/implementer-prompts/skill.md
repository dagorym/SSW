---
name: "implementer-prompts"
description: "Generate Coordinator-ready Implementer prompts for each subtask."
---

# Planner Implementer Prompts

Load this skill only when composing per-subtask Implementer prompts.

## Required Content

Each Implementer prompt must:
- begin with `Your role is 'implementer'. Your task is as follows:`
- include allowed files
- state the task to implement
- include implementation-outcome acceptance criteria
- include relevant validation guidance
- include Tester test-file location guidance
- include repository-root-relative artifact guidance
- instruct the Implementer to continue past preflight when blockers are absent
- include the explicit completion gate:
  - `Do not report success unless all required artifacts exist and all changes are committed.`

## Limits

- Keep the prompt usable as-is by the Coordinator without reinterpretation.
