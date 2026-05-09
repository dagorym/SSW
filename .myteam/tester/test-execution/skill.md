---
name: "test-execution"
description: "Run tests and report structured pass/fail results tied to acceptance criteria."
---

# Tester Test Execution

Load this skill only when tests are ready to run or when results must be summarized to decide the next step.

## Required Actions

- Run the complete relevant suite or the smallest meaningful test scope selected for the task.
- Capture full output including passes, failures, and error messages.
- Summarize results in structured form.

## Structured Result Format

- total tests written or touched when relevant
- tests passed
- tests failed
- for each failure:
  - test name
  - acceptance criterion validated
  - expected behavior
  - actual behavior
  - error message

## Decision Support

- Use the results to determine whether testing should proceed, be refined, or stop on the failure path.
- Report the structured human-readable results before final commit handling.

## Limits

- Do not skip test execution after writing or modifying tests.
- Do not hide failing output details that affect the next decision.
